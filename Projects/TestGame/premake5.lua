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
        "Source/**.h", 
        "Source/**.cpp"
    }

    includedirs 
    {
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
