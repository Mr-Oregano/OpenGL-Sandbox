
function CreateDefaultProject(name)
	project(name)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/%{cfg.architecture}-%{cfg.buildcfg}-%{cfg.system}")
	objdir ("bin-int/%{cfg.architecture}-%{cfg.buildcfg}-%{cfg.system}")

	files {
		"include/**.h",
		"src/**.cpp",
		"**.glsl",
		"res/**"
	}

	includedirs {
		"include",
		"../../Core/src",
		"../../Core/vendor",
		"../../Core/vendor/spdlog/include",
		"../../Core/vendor/glm",
		"../../Core/vendor/Glad/include",
		"../../Core/vendor/imgui"
	}

	links "OpenGL-Core"
end

workspace "OpenGL-Sandbox"
	architecture "x64"
    flags "MultiProcessorCompile"

	configurations {
		"Debug",
		"Release"
	}
	
	defines "GLCORE_IMGUI"

    filter "configurations:Debug"
		symbols "On"
		optimize "Off"
		defines "GLSX_DEBUG"

	filter "configurations:Release"
		symbols "Off"
		optimize "On"
		defines "GLSX_RELEASE"

	filter {}
    
	group "Dependencies"
		include "Core"
	group ""
	
	-- TODO: Make this dynamic
	include "Examples/Abstraction"
	include "Examples/BatchRenderer"
	include "Examples/Effects"
	include "Examples/FontRendering"
	include "Examples/GeometryShader"
	include "Examples/Textures"
	include "Examples/Noise"

    
