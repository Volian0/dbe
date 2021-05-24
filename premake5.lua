workspace "dbe"
	configurations { "debug", "release" }

group "extern"
include "extern/glfw"
include "extern/glad"

group "projects"
include "engine"
include "sbox"
group ""
