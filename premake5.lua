workspace "CharacterIK"
	configurations { "Debug", "Release", "Dist" }
	platforms "x64"
	architecture "x86_64"
	startproject "Sandbox"

	filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "on"
		symbols "off"

	filter "configurations:Dist"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "on"
		symbols "off"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	----------------------------------------------------------------
	-- Projects
	group "Dependencies"
		include "vendor/raylib"
		include "vendor/Math3D/Math3D"
	group ""

include "CharacterIK"

group "Samples"
include "Samples/Sandbox"
include "Samples/ThreeBoneChain"
include "Samples/ConstraintPlane"
include "Samples/ConstraintThreeBoneChain"
group ""


