project "sbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir "bin"
	objdir "obj"

	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"../engine/src"
	}

	links {
		"engine"
	}

	filter "configurations:debug"
		runtime "debug"
		symbols "on"
		defines { "DEBUG" }

	filter "configurations:release"
		runtime "release"
		optimize "on"
		defines { "RELEASE" }

	filter "system:linux"
		links {
			engine_system_deps_unix
		}

	filter "system:windows"
		links {
			engine_system_deps_windows
		}
