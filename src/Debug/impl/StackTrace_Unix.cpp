// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Debug/StackTrace.h"

#include <cstdlib>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <regex>

using rainbow::debug::StackTrace;

namespace
{
    constexpr int kFramesToCapture = 63;
    constexpr const char kFrameNameRegex[] = R"%(0x[0-9A-Fa-f]+\s+(\w+))%";
}

StackTrace rainbow::debug::stack_trace(int frames_to_skip)
{
    void* callstack[kFramesToCapture];
    const int count = backtrace(callstack, kFramesToCapture);

    StackTrace result;
    result.reserve(count - frames_to_skip);

    std::unique_ptr<char*[], decltype(&free)> frames(
        backtrace_symbols(callstack, count), free);
    std::cmatch match;
    std::regex name_rx(kFrameNameRegex);

    for (int i = frames_to_skip; i < count; ++i)
    {
        Dl_info info;
        if (dladdr(callstack[i], &info) && info.dli_sname)
        {
            int status = -1;
            auto demangled =
                abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            result.emplace_back(
                callstack[i], status == 0 ? demangled : info.dli_sname);
            free(demangled);
            continue;
        }

        if (std::regex_search(frames[i], match, name_rx))
            result.emplace_back(callstack[i], match[1].str());
    }

    return result;
}
