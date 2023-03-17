-- Dependencies 

local rootDir = "%{wks.location}/RockEngine/vendor/"

IncludeDir = {}
IncludeDir["GLFW"] = rootDir .. "GLFW/include"
IncludeDir["GLAD"] = rootDir .. "Glad/include"
IncludeDir["ImGui"] = rootDir .. "imgui"

LinksDir = {}
LinksDir["ImGui"] = rootDir .. "imgui/build/bin/Debug-windows-x86_64/ImGui/ImGui.lib"