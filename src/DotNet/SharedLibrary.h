// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_SHAREDLIBRARY_H_
#define COMMON_SHAREDLIBRARY_H_

#include <string>

#include "Common/NonCopyable.h"

namespace rainbow
{
    class SharedLibrary : private NonCopyable<SharedLibrary>
    {
    public:
        SharedLibrary(std::string path);
        ~SharedLibrary();

        auto path() const { return path_.c_str(); }

        auto get_symbol(const char* symbol) const -> void*;

        template <typename T>
        auto get_symbol(const char* symbol) const -> T
        {
            return reinterpret_cast<T>(get_symbol(symbol));
        }

        explicit operator bool() const { return handle_ != nullptr; }

    private:
        void* handle_;
        std::string path_;
    };
}

#endif
