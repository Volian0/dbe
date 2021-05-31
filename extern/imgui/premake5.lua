project "imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"

	includedirs {
		"include",
		"../glad/include",
		"../glfw/include"
	}

	files {
		"include/**.h",
		"src/**.cpp"
	}

	defines {
		"IMGUI_DISABLE_WIN32_FUNCTIONS"
	}

	filter "configurations:Debug"
		runtime "Debug"
		defines { "DEBUG" }
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		defines { "RELEASE" }
		optimize "on"
