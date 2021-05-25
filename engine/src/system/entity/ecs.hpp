#pragma once

#include <memory>

#include "entitymanager.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"

class ECS {
private:
	std::unique_ptr<ComponentManager> m_component_manager;
	std::unique_ptr<EntityManager> m_entity_manager;
	std::unique_ptr<SystemManager> m_system_manager;
public:
	ECS();

	EntityHandle new_entity();
	void destroy_entity(EntityHandle entity);

	template <typename T>
	void register_component() {
		m_component_manager->register_component<T>();
	}

	template <typename T>
	void add_component(EntityHandle entity, const T& component) {
		m_component_manager->add_component<T>(entity, component);

		auto signature = m_entity_manager->get_signature(entity);
		signature.set(m_component_manager->get_component_type<T>(), true);
		m_entity_manager->set_signature(entity, signature);

		m_system_manager->on_entity_signature_changed(entity, signature);
	}

	template <typename T>
	void remove_component(EntityHandle entity) {
		m_component_manager->remove_component<T>(entity);

		auto signature = m_entity_manager->get_signature(entity);
		signature.set(m_component_manager->get_component_type<T>(), false);
		m_entity_manager->set_signature(entity, signature);

		m_system_manager->on_entity_signature_changed(entity, signature);
	}

	template <typename T>
	T& get_component(EntityHandle entity) {
		return m_component_manager->get_component<T>(entity);
	}

	template <typename T>
	bool has_component(EntityHandle entity) {
		return m_entity_manager->get_signature(entity).
			test(m_component_manager->get_component_type<T>());
	}

	template <typename T>
	ComponentType get_component_type() {
		return m_component_manager->get_component_type<T>();
	}

	template <typename T>
	std::shared_ptr<T> register_system() {
		return m_system_manager->register_system<T>();
	}

	template <typename T>
	void set_system_signature(Signature signature) {
		m_system_manager->set_signature<T>(signature);
	}
};
