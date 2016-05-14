// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Debug/StackTrace.h"

#include <cinttypes>

#include "Platform/Macros.h"

#define STACK_FRAME_FMT       "    %2d: 0x%.16" PRIx64 " %s"
#define STACK_FRAME_LINE_FMT  " at %s:%d\n"

using rainbow::debug::StackTrace;

void rainbow::debug::dump_stack(int frames_to_skip)
{
    const auto& trace = stack_trace(frames_to_skip);
    const int count = static_cast<int>(trace.size());
    for (int i = 0; i < count; ++i)
    {
        const auto& frame = trace[i];
        const int index = count - i - 1;
        if (frame.filename.empty())
        {
            printf(STACK_FRAME_FMT "\n",  //
                   index,
                   reinterpret_cast<uint64_t>(frame.address),
                   frame.name.c_str());
        }
        else
        {
            printf(STACK_FRAME_FMT STACK_FRAME_LINE_FMT,
                   index,
                   reinterpret_cast<uint64_t>(frame.address),
                   frame.name.c_str(),
                   frame.filename.c_str(),
                   frame.line_number);
        }
    }
}

#if defined(RAINBOW_OS_ANDROID)
StackTrace rainbow::debug::stack_trace(int)
{
    return {};
}
#endif
