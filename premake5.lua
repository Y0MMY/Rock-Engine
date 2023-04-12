workspace "RockEngine"
	architecture "x64"
	targetdir "build"
	
	configurations 
	{ 
		"Debug", 
        "Release",
        "Dist"
    }
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Dependencies.lua"

group "Dependencies"
include "RockEngine/vendor/GLFW"
include "RockEngine/vendor/Glad"
include "RockEngine/vendor/imgui"
group ""

group "Core"
project "RockEngine"
    location "RockEngine"
    kind "StaticLib"
    language "C++"
    
	targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("build/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader("pch.h")
	pchsource("RockEngine/src/pch.cpp")

	files 
	{ 
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp",

		"%{prj.name}/vendor/yaml-cpp/src/**.cpp",
		"%{prj.name}/vendor/yaml-cpp/src/**.h",
		"%{prj.name}/vendor/yaml-cpp/include/**.h"
    }

    includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.yaml}",

	}

	links
	{
		"GLFW",
		"Glad",
		"imgui"
	}
    
	filter "files:RockEngine/vendor/yaml-cpp/src/**.cpp or files:RockEngine/vendor/ImGuiColorTextEdit/*.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		cppdialect "C++17"
        staticruntime "On"
        
		defines 
		{ 
            "RE_PLATFORM_WINDOWS"
		}
					
    filter "configurations:Debug"
        defines "RE_DEBUG"
		runtime "Debug"
        symbols "On"
group ""

group "Tools"
project "TheRock"
    location "TheRock"
    kind "ConsoleApp"
    language "C++"
    
	targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("build/bin-int/" .. outputdir .. "/%{prj.name}")

	dependson 
	{ 
		"RockEngine"
    }
    
	files 
	{ 
		"%{prj.name}/**.h", 
		"%{prj.name}/**.c", 
		"%{prj.name}/**.hpp", 
		"%{prj.name}/**.cpp" 
	}
    
	includedirs 
	{
        "%{prj.name}/src",
        "RockEngine/src",
        "RockEngine/vendor",
		"%{IncludeDir.glm}"
    }
	
	filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        
		links 
		{ 
			"RockEngine",
			"%{LinksDir.ImGui}",
			"%{LinksDir.assimp}"

		}
        
		defines 
		{ 
            "RE_PLATFORM_WINDOWS",
		}
    
   filter "configurations:Debug"
        defines "RE_DEBUG"
		runtime "Debug"
        symbols "On"
group ""   
  