#pragma once

#include <unordered_map>
#include <memory>
#include <assert.h>

#include "int.hpp"
#include "ecommon.hpp"
#include "componentarray.hpp"

/* Again, most of this can't be implemented in .cpp */
class ComponentManager {
private:
	std::unordered_map <const char*, ComponentType> m_component_types;
	std::unordered_map <const char*,
		std::shared_ptr <ComponentArrayBase>> m_component_arrays;

	ComponentType m_next_type;

	template <typename T>
	const std::shared_ptr <ComponentArray <T>> get_component_array() {
		const char* type_name = typeid(T).name();

		assert(m_component_types.find(type_name) != m_component_types.end()
			&& "Component not registered.");

		return std::static_pointer_cast
			<ComponentArray <T>>(m_component_arrays[type_name]);
	}
public:
	template <typename T>
	void register_component() {
		const char* type_name = typeid(T).name();

		assert(m_component_types.find(type_name) == m_component_types.end() &&
			"Component already registered");

		m_component_types.insert({type_name, m_next_type});
		m_component_arrays.insert({type_name, std::make_shared <ComponentArray <T>>()});

		m_next_type++;
	}

	template <typename T>
	ComponentType get_component_type() {
		const char* type_name = typeid(T).name();

		assert(m_component_types.find(type_name) != m_component_types.end()
			&& "Component not registered.");

		return m_component_types[type_name];
	}

	template <typename T>
	void add_component(EntityHandle entity, T component) {
		get_component_array<T>()->insert(entity, component);
	}

	template <typename T>
	void remove_component(EntityHandle entity) {
		get_component_array<T>()->remove(entity);
	}

	template <typename T>
	T& get_component(EntityHandle entity) {
		return get_component<T>()->get(entity);
	}

	void on_entity_destroy(EntityHandle entity);
};
