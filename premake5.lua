-- =========================================
-- RENDER BACKEND OPTIONS
-- =========================================
newoption
{
    trigger     = "use_vulkan",
    description = "Enable Vulkan rendering."
}

newoption
{
    trigger     = "use_opengl",
    description = "Enable OpenGL rendering."
}

newoption
{
    trigger     = "use_directx11",
    description = "Enable DirectX11 rendering."
}

-- =========================================
-- BACKEND SELECTION VALIDATION
-- =========================================
local selected_backends =
{
    vulkan    = _OPTIONS["use_vulkan"] or false,
    opengl    = _OPTIONS["use_opengl"] or false,
    directx11 = _OPTIONS["use_directx11"] or false
}

local selected_count = 0
for _, enabled in pairs(selected_backends) do
    if enabled then selected_count = selected_count + 1 end
end

if selected_count == 0 then
    error("No backend selected. Use one of: --use_vulkan, --use_opengl, --use_directx11.")
elseif selected_count > 1 then
    error("Only one backend can be selected at a time.")
end

-- Platform compatibility check
local osname = os.host()
if selected_backends.directx11 and osname ~= "windows" then
    error("DirectX11 backend is only supported on Windows.")
end

-- =========================================
-- WORKSPACE & GLOBAL CONFIGURATION
-- =========================================
workspace "DropEngine"
architecture "x64"
configurations { "Debug", "Release" }

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

filter {} -- reset all filters

outdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- =========================================
-- PROJECT: EngineLib (Static Library)
-- =========================================
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
    "%{prj.name}/src/Platform/Linux/*.c"
}
pic "on"

filter "system:windows"
files
{
    "%{prj.name}/include/Platform/Windows/*.h",
    "%{prj.name}/src/Platform/Windows/*.c"
}

-- Platform-specific source files
filter { "system:linux", "options:use_opengl" }
files
{
    "%{prj.name}/include/Graphics/OpenGL/Linux/*.h",
    "%{prj.name}/src/Graphics/OpenGL/Linux/*.c"
}

filter { "system:linux", "options:use_vulkan" }
files
{
    "%{prj.name}/include/Graphics/Vulkan/Linux/*.h",
    "%{prj.name}/src/Graphics/Vulkan/Linux/*.c"
}

filter { "system:windows", "options:use_opengl" }
files
{
    "%{prj.name}/include/Graphics/OpenGL/Windows/*.h",
    "%{prj.name}/src/Graphics/OpenGL/Windows/*.c",
    "%{prj.name}/vendor/opengl/**.h"
}

filter { "system:windows", "options:use_directx11" }
files
{
    "%{prj.name}/include/Graphics/DirectX11/Windows/*.h",
    "%{prj.name}/src/Graphics/DirectX11/Windows/*.c"
}

filter {}

-- =========================================
-- PROJECT: EngineDLL (Shared Library)
-- =========================================
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
links { "X11" }

filter "system:windows"
links { "user32", "gdi32" }

filter { "system:linux", "options:use_opengl" }
links { "EGL", "GL" }

filter { "system:linux", "options:use_vulkan" }
links { "vulkan" }

filter { "system:windows", "options:use_opengl" }
links { "opengl32" }

filter { "system:windows", "options:use_directx11" }
links { "d3d11", "dxgi", "dxguid" }

filter {}

-- =========================================
-- PROJECT: Test (Console App)
-- =========================================
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
