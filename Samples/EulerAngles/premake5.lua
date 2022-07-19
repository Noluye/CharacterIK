project "EulerAngles"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../../bin-int/" .. outputdir .. "/%{prj.name}")

	defines{ "_CRT_SECURE_NO_WARNINGS" }
	linkoptions { "-IGNORE:4099" }

	files {
		"src/**.h",
		"src/**.cpp",
		"../Core/src/**.h",
		"../Core/src/**.cpp",
		"premake5.lua"
	}

	includedirs
	{
		"src",
		"../Core/src",
		"../../vendor/raylib/src",
		"../../vendor/raylib/src/external/glfw/include",
		"../../vendor/raygui/src",
		"../../vendor/Math3D/Math3D/src",
		"../../CharacterIK/src"
	}
	-------------------------------------------------------------------
	-- raylib
	links { "raylib", "CharacterIK" }

	filter "system:windows"
		systemversion "latest"
		libdirs {
			"../../vendor/raylib/bin/" .. outputdir .. "/raylib"
		}
		dependson { "raylib" }
		links { "raylib.lib" }
