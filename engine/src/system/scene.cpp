#include "scene.hpp"

Entity::Entity() {

}

Entity::Entity(EntityHandle handle, Scene* scene) :
	handle(handle), scene(scene) {}

void Entity::destroy() {
	assert(scene != nullptr);

	scene->ecs.destroy_entity(handle);
}

Entity Scene::new_entity() {
	return Entity(ecs.new_entity(), this);
}
