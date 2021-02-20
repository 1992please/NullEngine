project "NullEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    location "%{wks.location}/Intermediate/ProjectFiles"
    targetdir ("%{wks.location}/Binaries/" ..outputdir.. "/%{prj.name}")
    objdir    ("%{wks.location}/Intermediate/Build/" ..outputdir.. "/%{prj.name}")

    files 
    {
        "Source/Runtime/**.h", 
        "Source/Runtime/**.cpp"
    }

    includedirs 
    {
        "Source/Runtime",
        "Source/ThirdParty/spdlog/include",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.gl3w}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "glfw",
        "gl3w",
        "imgui",
        "opengl32.lib"
    }

    pchheader "NullPCH.h"
    pchsource "Source/Runtime/NullPCH.cpp"

    filter "system:windows"
        systemversion "latest"
        defines 
        {
            "NE_WINDOWS"
        }
        disablewarnings
        {
            "4291" -- usingnew without delete
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

project "NullEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    location "%{wks.location}/Intermediate/ProjectFiles"
    targetdir ("%{wks.location}/Binaries/" ..outputdir.. "/%{prj.name}")
    objdir    ("%{wks.location}/Intermediate/Build/" ..outputdir.. "/%{prj.name}")

    files 
    {
        "Source/Editor/**.h", 
        "Source/Editor/**.cpp"
    }

    includedirs 
    {
        "%{wks.location}/Engine/Source/Editor",
        "%{wks.location}/Engine/Source/Runtime",
        "%{wks.location}/Engine/Source/ThirdParty"
    }

    links
    {
        "NullEngine"
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
