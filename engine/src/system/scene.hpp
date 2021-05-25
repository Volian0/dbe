#pragma once

#include "entity/ecs.hpp"

struct Entity;

class Scene {
private:

	friend class Entity;
public:
	ECS ecs; // TODO: make private
	Entity new_entity();
};

struct Entity {
	Scene* scene { nullptr };
	EntityHandle handle;

	Entity();
	Entity(EntityHandle handle, Scene* scene);
	Entity(const Entity& other) = default;

	void destroy();

	template <typename T>
	T& get_component() {
		assert(scene != nullptr);

		return scene->ecs.get_component<T>(handle);
	}

	template <typename T>
	void add_component(const T& c) {
		assert(scene != nullptr);

		return scene->ecs.add_component<T>(handle, c);
	}

	template <typename T>
	bool has_component() {
		assert(scene != nullptr);

		return scene->ecs.has_component<T>(handle);
	}
};
