#include "componentmanager.hpp"

void ComponentManager::on_entity_destroy(EntityHandle entity) {
	for (const auto& i : m_component_arrays) {
		const auto& c = i.second;
		c->on_entity_destroy(entity);
	}
}
