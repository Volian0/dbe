workspace "dbe"
	configurations { "debug", "release" }

group "extern"
include "extern/glfw"
include "extern/glad"
include "extern/physfs"
include "extern/assimp"

group "projects"
include "engine"
include "sbox"
group ""
