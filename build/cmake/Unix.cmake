if(ANDROID)
  unset(APPLE)
  unset(WIN32)
  if(NOT UNIX)
    set(UNIX 1)
  endif()
  set(USE_FMOD_STUDIO ON)
elseif(MINGW OR UNIX)
  if(UNIT_TESTS)
    set(RAINBOW_COMMON_CFLAGS "-frtti -fexceptions --coverage")
  elseif(APPLE)
    # TODO: Remove when Clang implements FileSystem TS.
    set(RAINBOW_COMMON_CFLAGS "-fno-rtti -fexceptions")
  else()
    set(RAINBOW_COMMON_CFLAGS "-fno-rtti -fno-exceptions")
  endif()
  if(MINGW)
    set(RAINBOW_CSTD   "-std=gnu11")
    set(RAINBOW_CXXSTD "-std=gnu++14")
  else()
    set(RAINBOW_CSTD   "-std=c11")
    set(RAINBOW_CXXSTD "-std=c++14")
    if(EMSCRIPTEN)
      set(USE_FMOD_STUDIO OFF)
    elseif(NOT APPLE)
      # Common C-/C++ compiler flags
      set(RAINBOW_COMMON_CFLAGS "${RAINBOW_COMMON_CFLAGS} -fstack-protector -fpie -march=native")
    endif()

    # Compiler warning flags
    set(RAINBOW_CXX_WARNINGS "-Werror -pedantic -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wsign-promo")
  endif()

  # Set CFLAGS and CXXFLAGS
  set(CMAKE_C_FLAGS   "-pipe ${RAINBOW_CSTD} -w ${RAINBOW_COMMON_CFLAGS}")
  set(CMAKE_CXX_FLAGS "-pipe ${RAINBOW_CXXSTD} ${RAINBOW_CXX_WARNINGS} ${RAINBOW_COMMON_CFLAGS}")

  if(APPLE)
    # TODO: Temporary workaround till Apple LLVM includes FileSystem TS.
    find_library(BOOST_FILESYSTEM boost_filesystem REQUIRED)
    find_library(BOOST_SYSTEM boost_system REQUIRED)
    find_library(CORESERVICES_LIBRARY CoreServices REQUIRED)
    set(PLATFORM_LIBRARIES ${CORESERVICES_LIBRARY} ${BOOST_FILESYSTEM} ${BOOST_SYSTEM})
  else()
    # Debug- and release-specific flags
    set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -ftrapv")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -D_FORTIFY_SOURCE=2")

    # Set LDFLAGS
    if(NOT MINGW)
      set(CMAKE_EXE_LINKER_FLAGS "-Wl,-z,now -Wl,-z,relro -pie -pthread")
      set(PLATFORM_LIBRARIES m rt stdc++fs)
    endif()
  endif()
endif()
