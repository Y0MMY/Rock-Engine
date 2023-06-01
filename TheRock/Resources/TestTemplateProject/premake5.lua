RootDirectory = os.getenv("THEROCK_DIR")

workspace "TestTemplate"
	architecture "x64"
	targetdir "build"
	
	configurations 
	{ 
		"Debug", 
        "Release",
    }
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "TestTemplate"
    location "TestTemplate"
    kind "SharedLib"
    language "C++"
    
	targetdir ("%{RootDirectory}/build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{RootDirectory}/build/bin-int/" .. outputdir .. "/%{prj.name}")
   
	files 
	{ 
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp" ,
		"!%{prj.name}/src/Resources/**"
	}
    
	includedirs 
	{
        "%{prj.name}/src",
        "Assets/",
        -- "%{RootDirectory}/RockEngine/src",
        -- "%{RootDirectory}/RockEngine/vendor",
		"%{RootDirectory}/RockEngine/vendor/glm",
    }
	
	filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        
		links 
		{ 
			-- "%{RootDirectory}/build/bin/" .. outputdir .. "/RockEngine/RockEngine",
			"%{RootDirectory}/RockEngine/vendor/Glad/build/bin/" .. outputdir .. "/Glad/Glad",
			"%{RootDirectory}/RockEngine/vendor/imgui/build/bin/" .. outputdir .. "/imgui/ImGui.lib",
			"%{RootDirectory}/RockEngine/vendor/GLFW/bin/" .. outputdir .. "/GLFW/GLFW.lib",
			"%{RootDirectory}/RockEngine/vendor/assimp/bin/Debug/assimp-vc142-mtd",
			"Assets/libs/RockEngine-ScriptCore.lib"
		}
        
		defines 
		{ 
            "RE_PLATFORM_WINDOWS",
		}
    
		postbuildcommands
		{
			"{COPY} %{RootDirectory}/build/bin/" .. outputdir .. "/%{prj.name}/*.dll %{wks.location}/../Scripts"
		}

   filter "configurations:Debug"
        defines "RE_DEBUG"
		runtime "Debug"
        symbols "On"
  