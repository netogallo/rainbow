// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_CORESCRIPT_H_
#define DOTNET_CORESCRIPT_H_

#include "DotNet/Core.h"
#include "DotNet/SharedLibrary.h"
#include "Input/InputListener.h"
#include "Script/GameBase.h"

class CoreScript final : public rainbow::GameBase, public InputListener
{
public:
    static const CoreScript* instance() { return s_instance; }

    CoreScript(rainbow::Director&);
    ~CoreScript() override;

    auto propagate_animation_event() const
    {
        return propagate_animation_event_;
    }

private:
    using InitFn = void (*)(int32_t, int32_t);
    using ControllerButtonEventFn = void (*)(rainbow::ControllerButtonEvent);
    using ControllerEventFn = void (*)(uint32_t);
    using ControllerMotionEventFn = void (*)(rainbow::ControllerAxisMotion);
    using KeyEventHandlerFn = void (*)(rainbow::KeyStroke);
    using MemoryWarningEventHandlerFn = void (*)();
    using PointerEventHandlerFn = void (*)(const Pointer*, uint32_t);
    using PropagateAnimationEventFn = void (*)(void*, int32_t, uint32_t);
    using UpdateFn = void (*)(uint64_t);

    static CoreScript* s_instance;

    UpdateFn update_;
    ControllerMotionEventFn on_controller_axis_motion_;
    ControllerButtonEventFn on_controller_button_down_;
    ControllerButtonEventFn on_controller_button_up_;
    ControllerEventFn on_controller_connected_;
    ControllerEventFn on_controller_disconnected_;
    KeyEventHandlerFn on_key_down_;
    KeyEventHandlerFn on_key_up_;
    PointerEventHandlerFn on_pointer_moved_;
    PointerEventHandlerFn on_pointer_began_;
    PointerEventHandlerFn on_pointer_ended_;
    PointerEventHandlerFn on_pointer_canceled_;
    PropagateAnimationEventFn propagate_animation_event_;
    MemoryWarningEventHandlerFn on_memory_warning_;
    InitFn init_;
    rainbow::SharedLibrary core_clr_;
    coreclr::ShutdownFn shutdown_;
    void* host_handle_;
    unsigned int domain_id_;

    // Implement GameBase.

    void init_impl(const Vec2i& screen) override;
    void update_impl(unsigned long dt) override;
    void on_memory_warning_impl() override;

    // Implement InputListener.

    bool on_controller_axis_motion_impl(
        const rainbow::ControllerAxisMotion&) override;
    bool on_controller_button_down_impl(
        const rainbow::ControllerButtonEvent&) override;
    bool on_controller_button_up_impl(
        const rainbow::ControllerButtonEvent&) override;
    bool on_controller_connected_impl(unsigned int id) override;
    bool on_controller_disconnected_impl(unsigned int id) override;

    bool on_key_down_impl(const rainbow::KeyStroke&) override;
    bool on_key_up_impl(const rainbow::KeyStroke&) override;

    bool on_pointer_began_impl(const ArrayView<Pointer>&) override;
    bool on_pointer_canceled_impl() override;
    bool on_pointer_ended_impl(const ArrayView<Pointer>&) override;
    bool on_pointer_moved_impl(const ArrayView<Pointer>&) override;
};

#endif
