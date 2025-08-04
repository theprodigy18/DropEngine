workspace "DropEngine"
architecture "x64"
configurations
{
    "Debug",
    "Release"
}

filter "configurations:Debug"
defines "DEBUG"
symbols "on"

filter "configurations:Release"
defines "RELEASE"
optimize "on"

filter "system:linux"
defines "PLATFORM_LINUX"

filter "system:windows"
defines "PLATFORM_WINDOWS"
systemversion "latest"

filter { "action:gmake", "configurations:Release" }
buildoptions { "-static-libgcc", "-static-libstdc++" }
linkoptions { "-static-libgcc", "-static-libstdc++" }

filter { "action:vs*", "configurations:Release" }
staticruntime "on"

filter {} -- Reset all filters to avoid leaking.

outdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "EngineLib"
location "EngineLib"
kind "StaticLib"
language "C"
cdialect "C11"

targetdir("bin/" .. outdir .. "/data")
objdir("bin-int/" .. outdir .. "/data")

pchheader "pch.h"
pchsource "%{prj.name}/src/pch.c"

files
{
    "%{prj.name}/include/*.h",
    "%{prj.name}/include/Platform/*.h",
    "%{prj.name}/include/Graphics/*.h",
    "%{prj.name}/src/*.c"
}
includedirs
{
    "%{prj.name}/include",
	"%{prj.name}/vendor"
}

filter "system:linux"
files
{
    "%{prj.name}/include/Platform/Linux/*.h",
    "%{prj.name}/include/Graphics/OpenGL/Linux/*.h",
    "%{prj.name}/src/Platform/Linux/*.c",
    "%{prj.name}/src/Graphics/OpenGL/Linux/*.c"
}
pic "on"

filter "system:windows"
files
{
    "%{prj.name}/include/Platform/Windows/*.h",
    "%{prj.name}/include/Graphics/OpenGL/Windows/*.h",
    "%{prj.name}/src/Platform/Windows/*.c",
    "%{prj.name}/src/Graphics/OpenGL/Windows/*.c",
	"%{prj.name}/vendor/opengl/**.h",
}

filter {}

project "EngineDLL"
location "EngineDLL"
kind "SharedLib"
language "C"
cdialect "C11"

targetdir("bin/" .. outdir .. "/data")
objdir("bin-int/" .. outdir .. "/data")

files
{
    "%{prj.name}/include/*.h",
    "%{prj.name}/src/*.c"
}
includedirs
{
    "%{prj.name}/include",
    "EngineLib/include"
}
links
{
    "EngineLib"
}
defines "DLL_EXPORTS"

filter "system:linux"
links
{
    "X11",
    "EGL",
    "GL"
}

filter "system:windows"
links
{
    "user32",
	"gdi32",
	"opengl32"
}

filter {}

project "Test"
location "Test"
kind "ConsoleApp"
language "C"
cdialect "C11"

targetdir("bin/" .. outdir)
objdir("bin-int/" .. outdir)

files
{
    "%{prj.name}/*.h",
    "%{prj.name}/*.c"
}
includedirs
{
    "EngineDLL"
}
links
{
    "EngineDLL"
}

filter "system:linux"
postbuildcommands
{
    "{COPY} %{cfg.targetdir}/data/libEngineDLL.so %{cfg.targetdir}/"
}

filter "system:windows"
postbuildcommands
{
    "{COPY} %{cfg.targetdir}/data/EngineDLL.dll %{cfg.targetdir}/"
}

filter {}
