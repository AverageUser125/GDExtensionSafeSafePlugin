#!/usr/bin/env python
import os
import sys

# export OSXCROSS_ROOT="$HOME/osxcross"
# scons platform=macos osxcross_sdk=darwin24.1

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
# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags
# - LINKFLAGS are for linking flags


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
