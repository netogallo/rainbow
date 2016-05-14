// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Debug/StackTrace.h"

#include <experimental/filesystem>

#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

namespace filesystem = std::experimental::filesystem;

using rainbow::debug::StackTrace;

namespace
{
    constexpr int kFramesToCapture = 63;
    constexpr size_t kSymbolInfoSize = sizeof(SYMBOL_INFO) + (MAX_SYM_NAME - 1);
}

StackTrace rainbow::debug::stack_trace(int frames_to_skip)
{
    StackTrace result;

    auto process = GetCurrentProcess();
    if (SymInitialize(process, nullptr, TRUE) == FALSE)
    {
        // auto error = GetLastError();
        return result;
    }

    void* backtrace[kFramesToCapture];
    const unsigned int frame_count = CaptureStackBackTrace(
        frames_to_skip, kFramesToCapture, backtrace, nullptr);

    TCHAR buffer[kSymbolInfoSize];
    auto symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
    symbol->SizeOfStruct = sizeof(*symbol);
    symbol->MaxNameLen = MAX_SYM_NAME;

    result.reserve(frame_count);

    for (unsigned int i = 0; i < frame_count; ++i)
    {
        DWORD64 address = reinterpret_cast<DWORD64>(backtrace[i]);
        DWORD64 sym_displacement = 0;
        SymFromAddr(process, address, &sym_displacement, symbol);

        DWORD line_displacement = 0;
        IMAGEHLP_LINE64 line{};
        line.SizeOfStruct = sizeof(line);
        SymGetLineFromAddr64(process, address, &line_displacement, &line);

        result.emplace_back(
            reinterpret_cast<void*>(symbol->Address + sym_displacement),
            symbol->Name,
            line.FileName == nullptr
                ? ""
                : filesystem::path(line.FileName).filename().string(),
            line.LineNumber);
    }

    return result;
}
