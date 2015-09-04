#pragma once

#ifdef defined(_WIN64)
#  define HOMM3TOOL_WINDOWS
#  define HOMM3TOOL_WINDOWS_64
#elif defined(_WIN32)
#  define HOMM3TOOL_WINDOWS
#  define HOMM3TOOL_WINDOWS_32
#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
#  define HOMM3TOOL_UNIX
#else
#  error "homm3Tool doesn't support this target"
#endif

#ifdef HOMM3TOOL_WINDOWS
#  ifdef __GNUC__
#    define DLL_IMPORT __attribute__((dllimport))
#    define DLL_EXPORT __attribute__((dllexport))
#  else
#    define DLL_IMPORT __declspec(dllimport)
#    define DLL_EXPORT __declspec(dllexport)
#  endif
#else
#  ifdef __GNUC__
#    define DLL_IMPORT  __attribute__((visibility("default")))
#    define DLL_EXPORT __attribute__((visibility("default")))
#    define ELF_VISIBILITY __attribute__((visibility("default")))
#    define ELF_VISIBILITY __attribute__((visibility("default")))
#  endif
#endif

#ifdef HOMM3TOOL_DLL
#  define DLL_LINK DLL_EXPORT
#else
#  define DLL_LINK DLL_IMPORT
#endif