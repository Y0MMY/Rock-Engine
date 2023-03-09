workspace "RockEngine"
	architecture "x64"
	targetdir "build"
	
	configurations 
	{ 
		"Debug", 
        "Release",
        "Dist"
    }
    
local outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
project "RockEngine"
    location "RockEngine"
    kind "StaticLib"
    language "C++"
    
	targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("build/bin-int/" .. outputdir .. "/%{prj.name}")

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
		"%{prj.name}/vendor",
	}
    
	filter "system:windows"
		cppdialect "C++17"
        staticruntime "On"
        
		defines 
		{ 
            "RE_PLATFORM_WINDOWS"
		}
					
    filter "configurations:Debug"
        defines "RE_DEBUG"
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
    }
	
	filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        
		links 
		{ 
			"RockEngine"
		}
        
		defines 
		{ 
            "RE_PLATFORM_WINDOWS",
		}
    
    filter "configurations:Debug"
        defines "RE_DEBUG"
        symbols "On"
group ""   
  