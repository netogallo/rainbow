// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/SharedLibrary.h"

#include "Common/Logging.h"
#ifdef RAINBOW_OS_WINDOWS
#   include <Windows.h>
#   define RTLD_NOW    2
#   define RTLD_LOCAL  0
#else
#   include <dlfcn.h>
#endif

using rainbow::SharedLibrary;

namespace
{
#ifdef RAINBOW_OS_WINDOWS
    void dlclose(void* handle) { FreeLibrary(static_cast<HMODULE>(handle)); }

    void* dlopen(const char* filename, int) { return LoadLibrary(filename); }

    void* dlsym(void* handle, const char* symbol)
    {
        return GetProcAddress(static_cast<HMODULE>(handle), symbol);
    }
#endif
}

SharedLibrary::SharedLibrary(std::string path) : path_(std::move(path))
{
    handle_ = dlopen(path_.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle_ == nullptr)
    {
#ifdef RAINBOW_OS_WINDOWS
        char* message;
        DWORD_PTR args[]{reinterpret_cast<DWORD_PTR>(path_.c_str())};
        FormatMessage(  //
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_ARGUMENT_ARRAY,
            nullptr,
            GetLastError(),
            0,
            reinterpret_cast<LPTSTR>(&message),
            0,
            reinterpret_cast<va_list*>(args));
        LOGF("%s", message);
        HeapFree(GetProcessHeap(), 0, message);
#else
        LOGF("%s", dlerror());
#endif
    }
}

SharedLibrary::~SharedLibrary()
{
    if (handle_ == nullptr)
        return;

    dlclose(handle_);
}

auto SharedLibrary::get_symbol(const char* symbol) const -> void*
{
    return dlsym(handle_, symbol);
}
