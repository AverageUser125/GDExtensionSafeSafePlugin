#!/usr/bin/env python
import os
import sys

# export OSXCROSS_ROOT="$HOME/osxcross"
# scons platform=macos osxcross_sdk=darwin24.1

# clang-tidy.exe --config-file=".clang-tidy" .\src\*.cpp .\src\*.h
# just use compiledb=yes when calling scons

def find_folders(directory: str) -> list[str]:
    directories = [directory]
    for root, dirs, files in os.walk(directory):
        directories.extend([os.path.join(directory, os.path.relpath(os.path.join(root, d), directory)).replace("\\", "/") + "/" for d in dirs])
    return directories

def find_files_recursive(directory: str, filter_string: str) -> list:
    directories = find_folders(directory)
    sources = []
    for d in directories:
        sources += Glob(d + filter_string)  # Collects matching files
    return sources

SetOption('implicit_cache', 1)
env = SConscript("godot-cpp/SConstruct") 
env.Decider('MD5-timestamp')

# fails cause gdextension uses std::array in UtilityFunctions apparantly
# env.Append(LINKFLAGS="/NODEFAULTLIB")

env.Tool("compilation_db")
env.Alias("compiledb", env.CompilationDatabase())
env['COMPILATIONDB_USE_ABSPATH'] = 1

compiler = env['CC']
print("Using the following compiler: " + compiler)
if compiler == 'cl':
    env.Append(CPPDEFINES=[("_HAS_EXCEPTIONS", 0)])
    env.Append(CXXFLAGS=["/EHs-", "/std:c++17"])
    env.Append(LINKFLAGS=["/OPT:REF", "/DEFAULTLIB:kernel32.lib", "/DEFAULTLIB:libucrt.lib","/DEFAULTLIB:libvcruntime.lib", "/DEFAULTLIB:LIBCMT.lib", "/DEFAULTLIB:libcpmt.lib"])
elif 'clang' in compiler:
    env.Prepend(CFLAGS=["-std=gnu11"])
    env.Append(CXXFLAGS=['-fno-cxx-exceptions', '-fno-exceptions', "-std=gnu++17"])
    env.Append(LINKFLAGS=["-nolibc","-nostdlib","-nodefaultlibs"])
elif 'gcc' in compiler:
    env.Prepend(CFLAGS=["-std=gnu11"])
    env.Append(CXXFLAGS=['-fno-exceptions', '-fpermissive', "-std=gnu++17"])
    env.Append(LINKFLAGS=["-nolibc","-nodefaultlibs"])
elif 'emcc' in compiler:
    env.Prepend(CFLAGS=["-std=gnu11"])
    env.Append(CXXFLAGS=['-fno-exceptions', '-fpermissive', "-std=gnu++17"])
    env.Append(LINKFLAGS=["-nolibc", "-nodefaultlibs"])

# tweak this if you want to use different folders, or more folders, to store your source code in.
sources = find_files_recursive("src/", "*.cpp")
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/libSafeSave.{}.{}.framework/libSafeSave.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/bin/libSafeSave{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
