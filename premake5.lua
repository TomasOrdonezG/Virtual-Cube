workspace "main"
    architecture "x86_64"
    configurations { "Debug", "Release" }

project "main"
    kind "ConsoleApp"
    language "C++"

    targetdir "bin"
    objdir "obj"
    files { "src/**.h", "src/**.cpp" }

    includedirs { "/usr/include" }
    libdirs { "/usr/lib" }
    links { "glfw", "GLEW", "GL", "m" }

    postbuildcommands { "./bin/main" }

filter "configurations:Release"
    optimize "On"  