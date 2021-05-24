engine_deps = {
	"glfw",
	"glad"
}

engine_system_deps_windows = {
	"opengl32"
}

engine_system_deps_unix = {
	"pthread",
	"dl",
	"X11",
	"GL"
}

project "engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"

	targetdir "bin"
	objdir "obj"

	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"../extern/glfw/include",
		"../extern/glad/include",
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
