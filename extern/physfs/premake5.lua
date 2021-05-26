project "physfs"
	kind "StaticLib"
	language "C"

	files {
		"src/**.c",
		"src/**.h"
	}

	filter "system:linux"
		pic "on"

	filter "configurations:debug"
		defines { "DEBUG" }
		runtime "debug"
		symbols "on"

	filter "configurations:release"
		defines { "RELEASE" }
		runtime "release"
		optimize "on"
