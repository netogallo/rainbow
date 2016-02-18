// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/Core.h"

#include "Common/String.h"
#include "FileSystem/Path.h"
#include "Platform/Macros.h"

#ifdef RAINBOW_OS_MACOS
#   define BOOST_FILESYSTEM_NO_DEPRECATED
#   ifdef __GNUC__
#      pragma GCC diagnostic push
#      pragma GCC diagnostic ignored "-Wold-style-cast"
#   endif
#   include <boost/filesystem.hpp>
#   ifdef __GNUC__
#      pragma GCC diagnostic pop
#   endif
namespace filesystem = boost::filesystem;
#else
#   include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif

#define DOTNET_VERSION "1.0.1"

namespace
{
#if defined(RAINBOW_OS_MACOS)
    constexpr char kCoreCLRLibrary[] = "libcoreclr.dylib";
    constexpr char kCoreCLRPath[] =
        "/usr/local/share/dotnet/shared/Microsoft.NETCore.App/" DOTNET_VERSION;
#elif defined(RAINBOW_OS_UNIX)
    constexpr char kCoreCLRLibrary[] = "libcoreclr.so";
    constexpr char kCoreCLRPath[] =
        "/usr/local/share/dotnet/shared/Microsoft.NETCore.App/" DOTNET_VERSION;
#elif defined(RAINBOW_OS_WINDOWS)
    constexpr char kCoreCLRLibrary[] = "coreclr.dll";
    constexpr char kCoreCLRPath[] =
        "C:\\Program Files\\dotnet\\shared\\Microsoft.NETCore.App\\"
        DOTNET_VERSION;
#else
#   error "Unsupported platform"
#endif
}

std::string coreclr::core_library_path()
{
    return (filesystem::path{kCoreCLRPath} / kCoreCLRLibrary).string();
}

std::string coreclr::native_dll_search_directories()
{
    return kCoreCLRPath;
}

std::string coreclr::trusted_platform_assemblies(const std::string& search_path)
{
    std::string trusted_assemblies;
    std::string il_assemblies;

    for (auto&& entry : filesystem::directory_iterator{search_path})
    {
        if (!filesystem::is_regular_file(entry.status()))
            continue;

        auto&& path = entry.path();
        if (path.extension() == ".dll")
        {
            const auto& stem = path.stem();
            if (rainbow::ends_with(rainbow::string_view{stem.string()}, ".ni"))
                trusted_assemblies += path.string() + ":";
            else
                il_assemblies += path.string() + ":";
        }
    }

    const size_t total = trusted_assemblies.length() + il_assemblies.length();
    trusted_assemblies.reserve(total);
    trusted_assemblies += il_assemblies;

    if (!trusted_assemblies.empty())
        trusted_assemblies.pop_back();

    return trusted_assemblies;
}
