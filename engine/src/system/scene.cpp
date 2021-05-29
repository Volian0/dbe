#include "scene.hpp"
#include "transform.hpp"

Entity::Entity() {

}

Entity::Entity(EntityHandle handle, Scene* scene) :
	handle(handle), scene(scene) {}

void Entity::destroy() {
	assert(scene != nullptr);

	scene->m_ecs.destroy_entity(handle);
}

Entity Scene::new_entity() {
	return Entity(m_ecs.new_entity(), this);
}

Scene::Scene() {
	/* Register components */
	m_ecs.register_component<Shader>();
	m_ecs.register_component<Mesh>();
	m_ecs.register_component<Transform>();
	m_ecs.register_component<Sun>();
	m_ecs.register_component<Material>();

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
}

void Scene::render(const vec2& fb_size) {
	m_light_renderer->render();
	m_renderer->render(fb_size);
}
