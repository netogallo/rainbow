// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/CoreScript.h"

#include "DotNet/Rainbow/Interop/UMaster.h"
#include "FileSystem/FileSystem.h"

#define kFailedCoreCLR "Failed to load CoreCLR"
#define kFailedCreateDelegate                                                  \
    "CoreCLR: Failed to load '%s.%s' from assembly '%s'"
#define kFailedLoadFunction "Failed to load '%s' from '%s'"

#define LOG_ERROR_WITH(logx)                                                   \
    do                                                                         \
    {                                                                          \
        if (error)                                                             \
        {                                                                      \
            logx(kFailedCreateDelegate ": %s",                                 \
                 type,                                                         \
                 method_name,                                                  \
                 kEntryPointAssembly,                                          \
                 error.get());                                                 \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            logx(kFailedCreateDelegate " (HRESULT: 0x%x)",                     \
                 type,                                                         \
                 method_name,                                                  \
                 kEntryPointAssembly,                                          \
                 result);                                                      \
        }                                                                      \
    } while (false)

using coreclr::CreateDelegateFn;
using coreclr::InitializeFn;
using coreclr::ShutdownFn;
using coreclr::kCreateDelegateFunction;
using coreclr::kInitializeFunction;
using coreclr::kShutdownFunction;
using rainbow::GameBase;

namespace
{
    constexpr char kApplicationDomain[] = "com.bifrost.rainbow";
    constexpr char kEntryPointAssembly[] = "rainbow-sharp";

    constexpr char kEntryPointAnimation[] = "Rainbow.Graphics.Animation";
    constexpr char kEntryPointInputManager[] = "Rainbow.Input.InputManager";
    constexpr char kEntryPointMain[] = "Rainbow.Program";

    constexpr char kEntryPointMethodInitialize[] = "Initialize";
    constexpr char kEntryPointMethodInitializeComponents[] =
        "InitializeComponents";
    constexpr char kEntryPointMethodPropagateEvent[] = "PropagateEvent";
    constexpr char kEntryPointMethodUpdate[] = "Update";

    constexpr char kOnControllerAxisMotion[] = "OnControllerAxisMotion";
    constexpr char kOnControllerButtonDown[] = "OnControllerButtonDown";
    constexpr char kOnControllerButtonUp[] = "OnControllerButtonUp";
    constexpr char kOnControllerConnected[] = "OnControllerConnected";
    constexpr char kOnControllerDisconnected[] = "OnControllerDisconnected";
    constexpr char kOnKeyDown[] = "OnKeyDown";
    constexpr char kOnKeyUp[] = "OnKeyUp";
    constexpr char kOnMemoryWarning[] = "OnMemoryWarning";
    constexpr char kOnPointerBegan[] = "OnPointerBegan";
    constexpr char kOnPointerCanceled[] = "OnPointerCanceled";
    constexpr char kOnPointerEnded[] = "OnPointerEnded";
    constexpr char kOnPointerMoved[] = "OnPointerMoved";

    enum class Requirement
    {
        Optional,
        Required,
    };

    bool failed(int status) { return status < 0; }

#ifdef RAINBOW_OS_WINDOWS
    auto format_message(DWORD message_id)
    {
        char* error = nullptr;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            nullptr,
            message_id,
            0,
            reinterpret_cast<LPSTR>(&error),
            0,
            nullptr);
        return std::unique_ptr<char[], decltype(&LocalFree)>{error, &LocalFree};
    }
#else
    auto format_message(int)
    {
        return std::unique_ptr<char[]>{};
    }
#endif

    bool create_delegate(CreateDelegateFn coreclr_create_delegate,
                         void* host_handle,
                         unsigned int domain_id,
                         const char* type,
                         const char* method_name,
                         void** delegate,
                         Requirement requirement)
    {
        const int result = coreclr_create_delegate(  //
            host_handle,
            domain_id,
            kEntryPointAssembly,
            type,
            method_name,
            delegate);
        const bool succeeded = !failed(result);
        if (!succeeded)
        {
            auto error = format_message(result);
            if (requirement == Requirement::Optional)
                LOG_ERROR_WITH(LOGW);
            else
                LOG_ERROR_WITH(LOGF);
        }
        return succeeded;
    }

    struct Delegate
    {
        const char* module;
        const char* name;
        void** delegate;
    };
}

CoreScript* CoreScript::s_instance = nullptr;

CoreScript::CoreScript(rainbow::Director& director)
    : GameBase(director), update_(nullptr), on_memory_warning_(nullptr),
      init_(nullptr), core_clr_(coreclr::core_library_path()),
      shutdown_(nullptr), host_handle_(nullptr), domain_id_(0)
{
    if (!core_clr_)
    {
        terminate(kFailedCoreCLR);
        return;
    }

    auto coreclr_initialize =
        core_clr_.get_symbol<InitializeFn>(kInitializeFunction);
    if (coreclr_initialize == nullptr)
    {
        LOGF(kFailedLoadFunction, kInitializeFunction, core_clr_.path());
        terminate(kFailedCoreCLR);
        return;
    }

    auto coreclr_create_delegate =
        core_clr_.get_symbol<CreateDelegateFn>(kCreateDelegateFunction);
    if (coreclr_create_delegate == nullptr)
    {
        LOGF(kFailedLoadFunction, kCreateDelegateFunction, core_clr_.path());
        terminate(kFailedCoreCLR);
        return;
    }

    shutdown_ = core_clr_.get_symbol<ShutdownFn>(kShutdownFunction);
    if (shutdown_ == nullptr)
    {
        LOGF(kFailedLoadFunction, kShutdownFunction, core_clr_.path());
        terminate(kFailedCoreCLR);
        return;
    }

    // Initialise CoreCLR
    {
        const std::string search_directories =
            coreclr::native_dll_search_directories();
        const std::string trusted_assemblies =
            coreclr::trusted_platform_assemblies(search_directories);

        const char* property_keys[]{
            "TRUSTED_PLATFORM_ASSEMBLIES",
            "APP_PATHS",
            "APP_NI_PATHS",
            "NATIVE_DLL_SEARCH_DIRECTORIES",
            "AppDomainCompatSwitch",
        };
        const char* property_values[]{
            trusted_assemblies.c_str(),
            rainbow::filesystem::current_path(),
            rainbow::filesystem::current_path(),
            search_directories.c_str(),
            "UseLatestBehaviorWhenTFMNotSpecified",  // AppDomainCompatSwitch
        };

        const int result = coreclr_initialize(  //
            rainbow::filesystem::executable_path(),
            kApplicationDomain,
            static_cast<int>(rainbow::array_size(property_keys)),
            property_keys,
            property_values,
            &host_handle_,
            &domain_id_);
        if (failed(result))
        {
            shutdown_ = nullptr;
            LOGF("CoreCLR: coreclr_initialize => HRESULT: 0x%x", result);
            terminate("Failed to initialise CoreCLR");
            return;
        }
    }

    using InitializeComponentsFn = void (*)(void*);
    InitializeComponentsFn initialize_components = nullptr;

    const auto delegates = {
        Delegate{kEntryPointMain,
                 kEntryPointMethodInitializeComponents,
                 reinterpret_cast<void**>(&initialize_components)},
        Delegate{kEntryPointMain,
                 kEntryPointMethodInitialize,
                 reinterpret_cast<void**>(&init_)},
        Delegate{kEntryPointMain,
                 kEntryPointMethodUpdate,
                 reinterpret_cast<void**>(&update_)},
        Delegate{kEntryPointInputManager,
                 kOnControllerAxisMotion,
                 reinterpret_cast<void**>(&on_controller_axis_motion_)},
        Delegate{kEntryPointInputManager,
                 kOnControllerButtonDown,
                 reinterpret_cast<void**>(&on_controller_button_down_)},
        Delegate{kEntryPointInputManager,
                 kOnControllerButtonUp,
                 reinterpret_cast<void**>(&on_controller_button_up_)},
        Delegate{kEntryPointInputManager,
                 kOnControllerConnected,
                 reinterpret_cast<void**>(&on_controller_connected_)},
        Delegate{kEntryPointInputManager,
                 kOnControllerDisconnected,
                 reinterpret_cast<void**>(&on_controller_disconnected_)},
        Delegate{kEntryPointInputManager,
                 kOnKeyDown,
                 reinterpret_cast<void**>(&on_key_down_)},
        Delegate{kEntryPointInputManager,
                 kOnKeyUp,
                 reinterpret_cast<void**>(&on_key_up_)},
        Delegate{kEntryPointInputManager,
                 kOnPointerBegan,
                 reinterpret_cast<void**>(&on_pointer_began_)},
        Delegate{kEntryPointInputManager,
                 kOnPointerCanceled,
                 reinterpret_cast<void**>(&on_pointer_canceled_)},
        Delegate{kEntryPointInputManager,
                 kOnPointerEnded,
                 reinterpret_cast<void**>(&on_pointer_ended_)},
        Delegate{kEntryPointInputManager,
                 kOnPointerMoved,
                 reinterpret_cast<void**>(&on_pointer_moved_)},
        Delegate{kEntryPointAnimation,
                 kEntryPointMethodPropagateEvent,
                 reinterpret_cast<void**>(&propagate_animation_event_)},
    };

    for (auto&& delegate : delegates)
    {
        if (!create_delegate(coreclr_create_delegate,
                             host_handle_,
                             domain_id_,
                             delegate.module,
                             delegate.name,
                             delegate.delegate,
                             Requirement::Required))
        {
            terminate("Failed to load assembly");
            return;
        }
    }

    create_delegate(coreclr_create_delegate,
                    host_handle_,
                    domain_id_,
                    kEntryPointMain,
                    kOnMemoryWarning,
                    reinterpret_cast<void**>(&on_memory_warning_),
                    Requirement::Optional);

    s_instance = this;

    R_ASSERT(initialize_components != nullptr, "Failed to load assembly?");

    rainbow::initialize_components(scenegraph(), initialize_components);
}

CoreScript::~CoreScript()
{
    if (!shutdown_)
        return;

    shutdown_(host_handle_, domain_id_);
}

void CoreScript::init_impl(const Vec2i& size)
{
    input().subscribe(this);
    init_(size.x, size.y);
}

void CoreScript::update_impl(unsigned long dt)
{
    update_(dt);
}

void CoreScript::on_memory_warning_impl()
{
    if (on_memory_warning_ == nullptr)
        return;

    on_memory_warning_();
}

bool CoreScript::on_controller_axis_motion_impl(
    const rainbow::ControllerAxisMotion& motion)
{
    on_controller_axis_motion_(motion);
    return true;
}

bool CoreScript::on_controller_button_down_impl(
    const rainbow::ControllerButtonEvent& event)
{
    on_controller_button_down_(event);
    return true;
}

bool CoreScript::on_controller_button_up_impl(
    const rainbow::ControllerButtonEvent& event)
{
    on_controller_button_up_(event);
    return true;
}

bool CoreScript::on_controller_connected_impl(unsigned int id)
{
    on_controller_connected_(id);
    return true;
}

bool CoreScript::on_controller_disconnected_impl(unsigned int id)
{
    on_controller_disconnected(id);
    return true;
}

bool CoreScript::on_key_down_impl(const rainbow::KeyStroke& key_stroke)
{
    on_key_down_(key_stroke);
    return true;
}

bool CoreScript::on_key_up_impl(const rainbow::KeyStroke& key_stroke)
{
    on_key_up_(key_stroke);
    return true;
}

bool CoreScript::on_pointer_began_impl(const ArrayView<Pointer>& pointers)
{
    on_pointer_began_(pointers.data(), static_cast<uint32_t>(pointers.size()));
    return true;
}

bool CoreScript::on_pointer_canceled_impl()
{
    on_pointer_canceled_(nullptr, 0);
    return true;
}

bool CoreScript::on_pointer_ended_impl(const ArrayView<Pointer>& pointers)
{
    on_pointer_ended_(pointers.data(), static_cast<uint32_t>(pointers.size()));
    return true;
}

bool CoreScript::on_pointer_moved_impl(const ArrayView<Pointer>& pointers)
{
    on_pointer_moved_(pointers.data(), static_cast<uint32_t>(pointers.size()));
    return true;
}

auto GameBase::create(rainbow::Director& director) -> std::unique_ptr<GameBase>
{
    return std::make_unique<CoreScript>(director);
}
