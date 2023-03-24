-- Dependencies 

local rootDir = "%{wks.location}/RockEngine/vendor/"

IncludeDir = {}
IncludeDir["GLFW"] = rootDir .. "GLFW/include"
IncludeDir["GLAD"] = rootDir .. "Glad/include"
IncludeDir["ImGui"] = rootDir .. "imgui"
IncludeDir["glm"] = rootDir .. "glm"
IncludeDir["stb"] = rootDir .. "stb/include"
IncludeDir["assimp"] = rootDir .. "assimp/include"

LinksDir = {}
LinksDir["ImGui"] = rootDir .. "imgui/build/bin/Debug-windows-x86_64/ImGui/ImGui.lib"