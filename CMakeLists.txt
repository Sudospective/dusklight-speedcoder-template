cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_TLS_CAINFO)
  foreach (_ca
    "$ENV{ProgramFiles}/Git/mingw64/ssl/certs/ca-bundle.crt"
    "$ENV{ProgramFiles}/Git/usr/ssl/certs/ca-bundle.crt"
    "C:/msys64/mingw64/ssl/certs/ca-bundle.crt"
    "C:/msys64/usr/ssl/certs/ca-bundle.crt"
    "/etc/ssl/certs/ca-certificates.crt"
    "/etc/pki/tls/certs/ca-bundle.crt")
    if (EXISTS "${_ca}")
      set(CMAKE_TLS_CAINFO "${_ca}" CACHE FILEPATH "CA bundle for FetchContent TLS")
      break()
    endif ()
  endforeach ()
endif ()

if (NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type")
endif ()

project(my_dst_mod VERSION 1.0.0 LANGUAGES CXX)

# The Dusklight version to build against: a release tag (e.g. "v2.0.0") or a commit SHA.
if (NOT DUSKLIGHT_VERSION)
    set(DUSKLIGHT_VERSION "v2.0.3")
endif ()
# Fetches the Dusklight sources into ./dusklight and provides DUSKLIGHT_DIR.
# Use -DDUSKLIGHT_DIR= to point at an existing Dusklight checkout.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchDusklight.cmake")
add_subdirectory("${DUSKLIGHT_DIR}/sdk" dusklight-sdk EXCLUDE_FROM_ALL)

configure_file(mod.json.in "${CMAKE_CURRENT_BINARY_DIR}/mod.json" @ONLY NEWLINE_STYLE LF)

if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif ()

add_mod(${PROJECT_NAME}
        FEATURES game fmt      # remove for service/asset-only mods; add webgpu for GfxService
        SOURCES
          src/dst.cpp
          src/mod.cpp
        MOD_JSON "${CMAKE_CURRENT_BINARY_DIR}/mod.json"
        RES_DIR res            # mod resources, including icon.png and banner.png
        OVERLAY_DIR overlay    # game file overlays; remove if unused
        TEXTURES_DIR textures  # texture replacements; remove if unused
)

target_include_directories(${PROJECT_NAME} PRIVATE SYSTEM "include" "src")

if (MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static)
endif ()
