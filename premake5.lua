include "Engine/Extras/PremakeCustomization.lua"

workspace "NullEngine"
  configurations { "Debug", "Release", "Shipping" }
  architecture "x86_64"
  startproject "TestGame"

outputdir = "%{cfg.buildcfg}_%{cfg.architecture}"
IncludeDir = {}
IncludeDir["GLFW"] = "Engine/Source/ThirdParty/glfw/include"
IncludeDir["GL3W"] = "Engine/Source/ThirdParty/gl3w/include"

  workspace_files 
  {
    "Engine/Extras/NullEngine.natvis",
    "Engine/Extras/TODO"
  }

include "Engine/Source/ThirdParty/glfw"
include "Engine/Source/ThirdParty/gl3w"

project "NE"
  kind "SharedLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"
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
    "Engine/Source/ThirdParty/spdlog/include",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.GL3W}"
  }

  links
  {
    "GLFW",
    "GL3W",
    "opengl32.lib"
  }

  filter "system:windows"
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
    optimize "on"

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


