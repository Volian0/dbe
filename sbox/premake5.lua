project "sbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	debugdir "../"

	targetdir "bin"
	objdir "obj"

	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"../extern/glfw/include",
		"../extern/glad/include",
		"../extern/physfs/src",
		"../extern/assimp/include",
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
			engine_deps,
			engine_system_deps_unix
		}

	filter "system:windows"
		links {
			engine_deps,
			engine_system_deps_windows
		}
