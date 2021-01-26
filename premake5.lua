include "Engine/Extras/PremakeCustomization.lua"

workspace "NullEngine"
	configurations { "Debug", "Release", "Shipping" }
	architecture "x86_64"
	startproject "TestGame"

outputdir = "%{cfg.buildcfg}_%{cfg.architecture}"
IncludeDir = {}
IncludeDir["glfw"] = "Engine/Source/ThirdParty/glfw/include"
IncludeDir["gl3w"] = "Engine/Source/ThirdParty/gl3w/include"
IncludeDir["imgui"] = "Engine/Source/ThirdParty/imgui"

	workspace_files 
	{
		"Engine/Extras/NullEngine.natvis",
		"Engine/Extras/TODO"
	}

include "Engine/Source/ThirdParty/glfw"
include "Engine/Source/ThirdParty/gl3w"
include "Engine/Source/ThirdParty/imgui"

project "NE"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	location "%{wks.location}/Intermediate/ProjectFiles"
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
		"%{IncludeDir.glfw}",
		"%{IncludeDir.gl3w}",
		"%{IncludeDir.imgui}"
	}

	links
	{
		"glfw",
		"gl3w",
		"imgui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"NE_WINDOWS"
		}

  filter "configurations:Debug"
    defines "NE_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "NE_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Shipping"
    defines "NE_SHIPPING"
    runtime "Release"
    optimize "on"

project "TestGame"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	location "%{wks.location}/Intermediate/ProjectFiles"
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
		systemversion "latest"
		defines 
		{ 
			"NE_WINDOWS"
		}

	filter "configurations:Debug"
		defines "NE_DEBUG"
    runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NE_RELEASE"
    runtime "Release"
		optimize "on"

	filter "configurations:Shipping"
		defines "NE_SHIPPING"
    runtime "Release"
		optimize "on"


