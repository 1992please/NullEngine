project "gl3w"
	kind "StaticLib"
	language "c"
	staticruntime "On"
	location "%{wks.location}/Intermediate/ProjectFiles"
	targetdir ("%{wks.location}/Binaries/" ..outputdir.. "/%{prj.name}")
	objdir    ("%{wks.location}/Intermediate/Build/" ..outputdir.. "/%{prj.name}")

	files
	{
		"include/GL/gl3w.h",
		"include/GL/glcorearb.h",
		"include/KHR/khrplatform.h",
		"src/gl3w.c",
	}

	includedirs
	{
		"include/"
	}

    filter "system:windows"
        systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Shipping"
		runtime "Release"
		optimize "on"