#pragma once

#include "entity/ecs.hpp"
#include "graphics/renderer.hpp"

struct Entity;

class Scene {
private:
	ECS m_ecs;

	friend class Entity;
public:
	std::shared_ptr <Renderer> m_renderer;
	std::shared_ptr <LightRenderer> m_light_renderer;

	Scene();

	Entity new_entity();

	void render(const vec2& fb_size);
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

		return scene->m_ecs.get_component<T>(handle);
	}

	template <typename T>
	void add_component(const T& c) {
		assert(scene != nullptr);

		return scene->m_ecs.add_component<T>(handle, c);
	}

	template <typename T>
	void remove_component() {
		scene->m_ecs.remove_component<T>(handle);
	}

	template <typename T>
	bool has_component() {
		assert(scene != nullptr);

		return scene->m_ecs.has_component<T>(handle);
	}
};
