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

		if (m_component_types.find(type_name) == m_component_types.end()) {
			log(LOG_WARNING, "Component not registered.");
			register_component<T>();
		}

		return std::static_pointer_cast
			<ComponentArray <T>>(m_component_arrays[type_name]);
	}
public:
	template <typename T>
	void register_component() {
		const char* type_name = typeid(T).name();

		if (m_component_types.find(type_name) != m_component_types.end()) {
			log(LOG_WARNING, "Component already registered");
			return;
		}

		m_component_types.insert({type_name, m_next_type});
		m_component_arrays.insert({type_name, std::make_shared <ComponentArray <T>>()});

		m_next_type++;
	}

	template <typename T>
	ComponentType get_component_type() {
		const char* type_name = typeid(T).name();

		if (m_component_types.find(type_name) == m_component_types.end()) {
			log(LOG_WARNING, "Component not registered");
			register_component<T>();
		}

		return m_component_types[type_name];
	}

	template <typename T>
	T& add_component(EntityHandle entity, const T& component) {
		get_component_array<T>()->insert(entity, component);
		return get_component_array<T>()->get(entity);
	}

	template <typename T>
	void remove_component(EntityHandle entity) {
		get_component_array<T>()->remove(entity);
	}

	template <typename T>
	T& get_component(EntityHandle entity) {
		return get_component_array<T>()->get(entity);
	}

	void on_entity_destroy(EntityHandle entity);
};
