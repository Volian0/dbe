#include <assert.h>

#include "entitymanager.hpp"

EntityManager::EntityManager() {
	for (u32 i = 0; i < MAX_ENTITIES; i++) {
		m_available_entities.push(i);
	}
}

EntityHandle EntityManager::create_entity() {
	assert(m_entity_count < MAX_ENTITIES && "Too many entities");

	EntityHandle id = m_available_entities.front();
	m_available_entities.pop();
	m_entity_count++;

	return id;
}

void EntityManager::destroy_entity(EntityHandle entity) {
	assert(entity < MAX_ENTITIES && "Invalid entity");

	m_signatures[entity].reset();

	m_available_entities.push(entity);
	m_entity_count--;
}

void EntityManager::set_signature(EntityHandle entity, Signature signature) {
	assert(entity < MAX_ENTITIES && "Invalid entity");

	m_signatures[entity] = signature;
}

Signature EntityManager::get_signature(EntityHandle entity) {
	assert(entity < MAX_ENTITIES && "Invalid entity");

	return m_signatures[entity];
}
