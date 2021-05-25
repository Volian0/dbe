#pragma once

/* All the rendering happens here
 * This is simply a system that gets registered
 * by the ECS. */

#include "system/entity/ecs.hpp"

class Renderer : public System {
private:
public:
	void render();
};
