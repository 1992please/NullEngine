include "Engine/Extras/PremakeCustomization.lua"

workspace "NullEngine"
	configurations { "Debug", "Release", "Shipping" }
	architecture "x86_64"
	startproject "NullEditor"

	workspace_files 
	{
		"Engine/Extras/NullEngine.natvis",
		"Engine/Extras/TODO"
	}

outputdir = "%{cfg.buildcfg}_%{cfg.architecture}"
IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/Engine/Source/ThirdParty/glfw/include"
IncludeDir["gl3w"] = "%{wks.location}/Engine/Source/ThirdParty/gl3w/include"
IncludeDir["imgui"] = "%{wks.location}/Engine/Source/ThirdParty/imgui"
IncludeDir["stb_image"] = "%{wks.location}/Engine/Source/ThirdParty/stb_image"



group "Engine/ThridParty"
include "Engine/Source/ThirdParty/glfw"
include "Engine/Source/ThirdParty/gl3w"
include "Engine/Source/ThirdParty/imgui"

group "Engine"
include "Engine"

group "Projects"
include "Projects/TestGame"
