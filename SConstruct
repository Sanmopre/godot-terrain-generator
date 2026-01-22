#!/usr/bin/env python
import os
import sys

env = SConscript("submodules/godot-cpp/SConstruct")

env.Append(CPPPATH=["src/"])
env.Append(CPPPATH=["submodules/FastNoiseLite/Cpp"])

sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "./bin/lib-terrain-generator.{}.{}.framework/lib-terrain-generator.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "./bin/lib-terrain-generator{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)