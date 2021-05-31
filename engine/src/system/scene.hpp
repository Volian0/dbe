#pragma once

#include "entity/ecs.hpp"
#include "graphics/renderer.hpp"

struct Entity;
class HierarchySystem;

class Scene {
private:
	ECS m_ecs;

	friend class Entity;
public:
	std::shared_ptr <Renderer> m_renderer;
	std::shared_ptr <LightRenderer> m_light_renderer;
	std::shared_ptr <HierarchySystem> m_hierarchy_system;

	Scene();

	Entity new_entity();

	void update();
	void render(const vec2& fb_size);
};

struct Entity {
	Scene* scene { nullptr };
	EntityHandle handle;

	Entity();
	Entity(EntityHandle handle, Scene* scene);
	Entity(const Entity& other) = default;

	void destroy();
	void destroy_children(EntityHandle parent);

	void add_child(Entity* child);
	void remove_child(Entity* child);
	void parent_to(Entity* parent);
	void unparent_from(Entity* parent);

	template <typename T>
	T& get_component() {
		assert(scene != nullptr);

		return scene->m_ecs.get_component<T>(handle);
	}

	template <typename T>
	T& add_component(const T& c) {
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

/* Responsible for updating entity parents and children */
struct Hierarchy {
	EntityHandle self { NULL_ENTITY };
	EntityHandle parent { NULL_ENTITY };
	std::vector <EntityHandle> children;
};

class HierarchySystem : public System {
private:
public:
	void update();
};
