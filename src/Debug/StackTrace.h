// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DEBUG_STACKTRACE_H_
#define DEBUG_STACKTRACE_H_

#include <string>
#include <utility>
#include <vector>

namespace rainbow { namespace debug
{
    struct StackFrame
    {
        void* address;
        std::string name;
        std::string filename;
        unsigned int line_number;

        StackFrame(void* address_, std::string name_)
            : StackFrame(address_, std::move(name_), "", 0)
        {
        }

        StackFrame(void* address_,
                   std::string name_,
                   std::string filename_,
                   unsigned int line_number_)
            : address(address_), name(std::move(name_)),
              filename(std::move(filename_)), line_number(line_number_)
        {
        }
    };

    using StackTrace = std::vector<StackFrame>;

    void dump_stack(int frames_to_skip = 2);
    StackTrace stack_trace(int frames_to_skip = 1);
}}  // namespace rainbow::debug

#endif
