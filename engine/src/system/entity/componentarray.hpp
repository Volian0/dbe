#pragma once

#include <array>
#include <unordered_map>

#include "int.hpp"
#include "ecommon.hpp"
#include "logger.hpp"

class ComponentArrayBase {
public:
	virtual ~ComponentArrayBase() = default;
	virtual void on_entity_destroy(EntityHandle entity) = 0;
};

/* This can't be implemented in .cpp, since it's a template. Ugh. */
template <typename T>
class ComponentArray : public ComponentArrayBase {
private:
	std::array <T, MAX_ENTITIES> m_component_array;

	std::unordered_map <EntityHandle, u32> m_entity_to_index;
	std::unordered_map <EntityHandle, u32> m_index_to_entity;

	u32 m_size;
public:
	void insert(EntityHandle entity, T component) {
		if (m_entity_to_index.find(entity) == m_index_to_entity.end()) {
			log(LOG_WARNING, "Attempting to add the same\
				component to an entity twice");
			return;
		}

		u32 new_index = m_size++;
		m_entity_to_index[entity] = new_index;
		m_index_to_entity[new_index] = entity;
		m_component_array[new_index] = component;
	}

	void remove(EntityHandle entity) {
		if (m_entity_to_index.find(entity) != m_entity_to_index.end()) {
			log(LOG_WARNING, "Attempting to remove a non-existant component");
			return;
		}

		u32 index_of_removed = m_entity_to_index[entity];
		u32 index_of_last = m_size - 1;
		m_component_array[index_of_removed] = m_component_array[index_of_last];

		EntityHandle entity_of_last = m_index_to_entity[index_of_last];
		m_entity_to_index[entity_of_last] = index_of_removed;
		m_index_to_entity[index_of_removed] = entity_of_last;

		m_entity_to_index.erase(entity);
		m_index_to_entity.erase(index_of_last);

		m_size--;
	}

	T& get(EntityHandle entity) {
		assert(m_entity_to_index.find(entity) != m_entity_to_index.end() &&
			"Getting a non-existant component");

		return m_component_array[m_entity_to_index[entity]];
	}

	void on_entity_destroy(EntityHandle entity) override {
		if (m_entity_to_index.find(entity) != m_entity_to_index.end()) {
			remove(entity);
		}
	}
};
