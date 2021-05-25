#include "ecs.hpp"

ECS::ECS() {
	m_component_manager = std::make_unique<ComponentManager>();
	m_entity_manager = std::make_unique<EntityManager>();
	m_system_manager = std::make_unique<SystemManager>();
}

EntityHandle ECS::new_entity() {
	return m_entity_manager->create_entity();
}

void ECS::destroy_entity(EntityHandle entity) {
	m_entity_manager->destroy_entity(entity);
	m_component_manager->on_entity_destroy(entity);
	m_system_manager->on_entity_destroy(entity);
}
