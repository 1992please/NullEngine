include "Engine/Extras/PremakeCustomization.lua"

workspace "NullEngine"
  configurations { "Debug", "Release", "Shipping" }
  architecture "x86_64"
  startproject "TestGame"

outputdir = "%{cfg.buildcfg}_%{cfg.architecture}"
IncludeDir = {}
IncludeDir["GLFW"] = "Engine/Source/ThirdParty/glfw/include"

  workspace_files 
  {
    "Engine/Extras/NullEngine.natvis",
    "Engine/Extras/TODO"
  }

include "Engine/Source/ThirdParty/glfw"

project "NE"
  kind "SharedLib"
  language "C++"
  location "Engine"
  targetname "NullEngine"
  targetdir ("%{wks.location}/Binaries/" ..outputdir.. "/%{prj.name}")
  objdir    ("%{wks.location}/Intermediate/Build/" ..outputdir.. "/%{prj.name}")

  files 
  {
    "Engine/Source/Runtime/**.h", 
    "Engine/Source/Runtime/**.cpp"
  }

  includedirs 
  { 
    "Engine/Source/Runtime",
    "Engine/Source/Runtime/Core",
    "Engine/Source/ThirdParty/spdlog/include",
    "%{IncludeDir.GLFW}"
  }

  links
  {
    "GLFW",
    "opengl32.lib"
  }

  filter "system:windows"
    cppdialect "C++17"
    staticruntime "on"
    systemversion "latest"
    defines 
    { 
      "NE_BUILD_DLL",
      "NE_WINDOWS"
    }

  filter "configurations:Debug"
    defines "NE_DEBUG"
    symbols "on"

  filter "configurations:Release"
    defines "NE_RELEASE"
    optimize "on"

  filter "configurations:Shipping"
    defines "NE_SHIPPING"
    optimize "off"

project "TestGame"
  kind "ConsoleApp"
  language "C++"
  location "Projects/%{prj.name}"
  targetdir ("%{wks.location}/Binaries/" ..outputdir.. "/%{prj.name}")
  objdir    ("%{wks.location}/Intermediate/Build/" ..outputdir.. "/%{prj.name}")

  files 
  {
    "Projects/%{prj.name}/Source/**.h", 
    "Projects/%{prj.name}/Source/**.cpp"
  }

  includedirs 
  { 
    "Engine/Source/ThirdParty/spdlog/include",
    "Engine/Source/Runtime",
    "Engine/Source/Runtime/Core"
  }

  links
  {
    "NE"
  }

  filter "system:windows"
    cppdialect "C++17"
    staticruntime "on"
    systemversion "latest"
    defines 
    { 
      "NE_WINDOWS"
    }
  postbuildcommands
  {
     ("copy %{wks.location}Binaries\\"..outputdir.."\\NE\\NullEngine.dll %{cfg.buildtarget.directory}")
  }

  filter "configurations:Debug"
    defines "NE_DEBUG"
    symbols "on"

  filter "configurations:Release"
    defines "NE_RELEASE"
    optimize "on"

  filter "configurations:Shipping"
    defines "NE_SHIPPING"
    optimize "on"


