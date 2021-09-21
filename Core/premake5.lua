
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "vendor/GLFW"
include "vendor/Glad"
include "vendor/imgui"

project "OpenGL-Core"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "glpch.h"
	pchsource "src/glpch.cpp"

	files {
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
	}

	defines "_CRT_SECURE_NO_WARNINGS"

	includedirs {
		"src",
		"vendor/spdlog/include",
		"vendor/GLFW/include",
		"vendor/Glad/include",
		"vendor/imgui",
		"vendor/glm",
		"vendor/stb_image"
	}

	links { 
		"GLFW",
		"Glad",
		"ImGui"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"GLCORE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "GLCORE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
		optimize "on"