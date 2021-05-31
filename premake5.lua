workspace "dbe"
	configurations { "debug", "release" }

group "extern"
include "extern/assimp"
include "extern/glad"
include "extern/glfw"
include "extern/imgui"
include "extern/physfs"

group "projects"
include "engine"
include "sbox"
group ""
