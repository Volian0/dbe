#include "systemmanager.hpp"

void SystemManager::on_entity_destroy(EntityHandle entity) {
	for (const auto& i : m_systems) {
		i.second->m_entities.erase(entity);
	}
}

void SystemManager::on_entity_signature_changed(EntityHandle entity, Signature signature) {
	for (const auto& i : m_systems) {
		const auto& type = i.first;
		const auto& sys = i.second;
		const auto& sys_sig = m_signatures[type];

		if ((signature & sys_sig) == sys_sig) {
			sys->m_entities.insert(entity);
		} else {
			sys->m_entities.erase(entity);
		}
	}
}
