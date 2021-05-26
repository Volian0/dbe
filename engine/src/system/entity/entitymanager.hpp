#pragma once

#include <queue>
#include <array>

#include "ecommon.hpp"

class EntityManager {
private:
	std::queue <EntityHandle> m_available_entities;
	std::array <Signature, MAX_ENTITIES> m_signatures;

	u32 m_entity_count { 0 };
public:
	EntityManager();

	EntityHandle create_entity();
	void destroy_entity(EntityHandle entity);
	void set_signature(EntityHandle entity, Signature signature);
	Signature get_signature(EntityHandle entity);
};
