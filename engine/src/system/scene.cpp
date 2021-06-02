#include <algorithm>

#include "transform.hpp"
#include "scene.hpp"

Entity::Entity() {

}

Entity::Entity(EntityHandle handle, Scene* scene) :
	handle(handle), scene(scene) {}

void Entity::destroy() {
	assert(scene != nullptr);

	if (has_component<Hierarchy>()) {
		destroy_children(handle);
	} else {
		scene->m_ecs.destroy_entity(handle);
	}
}

void Entity::destroy_children(EntityHandle parent) {
	if (scene->m_ecs.has_component<Hierarchy>(parent)) {
		auto& h = scene->m_ecs.get_component<Hierarchy>(parent);

		for (auto& c : h.children) {
			scene->m_ecs.destroy_entity(c);
		}
	}

	scene->m_ecs.destroy_entity(handle);
}

void Entity::add_child(Entity* child) {
	if (!child) {
		log(LOG_ERROR, "Unable to add a null child");
		return;
	}

	if (!has_component<Hierarchy>()) {
		add_component<Hierarchy>({handle});
	}

	if (!child->has_component<Hierarchy>()) {
		child->add_component<Hierarchy>({child->handle});
	}

	auto& hierarchy = get_component<Hierarchy>();
	auto& chierarchy = child->get_component<Hierarchy>();

	if (hierarchy.parent == child->handle ||
		std::find(hierarchy.children.begin(),
			hierarchy.children.end(),
			child->handle) != hierarchy.children.end()) {
		log(LOG_ERROR, "Cannot parent entity to one of it's children");
		return;
	}

	chierarchy.parent = handle;

	hierarchy.children.push_back(child->handle);
}

void Entity::remove_child(Entity* child) {
	if (!child) {
		log(LOG_ERROR, "Cannot remove null child.");
		return;
	}

	if (!child->has_component<Hierarchy>() ||
		!has_component<Hierarchy>()) {
		log(LOG_ERROR, "Cannot remove a child that doens't exist.");
		return;
	}

	auto& hierarchy = get_component<Hierarchy>();
	auto& chierarchy = child->get_component<Hierarchy>();

	hierarchy.children.erase(
		std::remove_if(hierarchy.children.begin(), hierarchy.children.end(),
			[&](EntityHandle c) {
				auto& a = scene->m_ecs.get_component<Hierarchy>(child->handle);
				auto& b = scene->m_ecs.get_component<Hierarchy>(c);

				return a.self == b.self;
			}),
		hierarchy.children.end()
	);

	chierarchy.parent = NULL_ENTITY;
}

void Entity::parent_to(Entity* parent) {
	parent->add_child(this);
}

void Entity::unparent_from(Entity* parent) {
	parent->remove_child(this);
}

Entity Scene::new_entity(std::string name) {
	Entity e = { m_ecs.new_entity(), this };

	e.add_component<Tag>({name});

	return e;
}

Scene::Scene() {
	/* Register components */
	m_ecs.register_component<Hierarchy>();
	m_ecs.register_component<Material>();
	m_ecs.register_component<Mesh>();
	m_ecs.register_component<Shader>();
	m_ecs.register_component<Sun>();
	m_ecs.register_component<Tag>();
	m_ecs.register_component<Transform>();

	/* Register the renderer */
	{
		m_renderer = m_ecs.register_system<Renderer>();
		Signature sig;
		sig.set(m_ecs.get_component_type<Transform>());
		sig.set(m_ecs.get_component_type<Shader>());
		sig.set(m_ecs.get_component_type<Mesh>());
		sig.set(m_ecs.get_component_type<Material>());
		m_ecs.set_system_signature<Renderer>(sig);
	}

	/* Register the renderer */
	{
		m_light_renderer = m_ecs.register_system<LightRenderer>();
		Signature sig;
		sig.set(m_ecs.get_component_type<Shader>());
		m_ecs.set_system_signature<LightRenderer>(sig);

		m_light_renderer->init(m_renderer.get());
	}

	/* Register the hierarchy system */
	{
		m_hierarchy_system = m_ecs.register_system<HierarchySystem>();
		Signature sig;
		sig.set(m_ecs.get_component_type<Transform>());
		sig.set(m_ecs.get_component_type<Hierarchy>());
		m_ecs.set_system_signature<HierarchySystem>(sig);
	}
}

void Scene::update(double timestep) {
	m_hierarchy_system->update();
}

void Scene::render(const vec2& fb_size, RenderTarget* fb) {
	m_light_renderer->render();
	m_renderer->render(fb_size, fb);
}

void HierarchySystem::update() {
	for (const auto& entity : m_entities) {
		auto& transform = m_ecs->get_component<Transform>(entity);
		auto& hierarchy = m_ecs->get_component<Hierarchy>(entity);

		if (hierarchy.parent != NULL_ENTITY) {
			transform.parent = &m_ecs->get_component<Transform>(hierarchy.parent);
		} else {
			transform.parent = nullptr;
		}
	}
}
