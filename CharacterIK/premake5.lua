project "CharacterIK"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "cikpch.h"
	pchsource "src/cikpch.cpp"

	files { "src/**.cpp", "src/**.h", "premake5.lua" }

	includedirs {
		"src",
		"../vendor/Math3D/Math3D/src"
	}

	links { "Math3D" }

	filter "system:windows"
		systemversion "latest"
		libdirs {"../vendor/Math3D/bin/" .. outputdir .. "/Math3D"}
		dependson { "Math3D" }
		links { "Math3D.lib" }