#pragma once

#include <assert.h>
#include <memory>
#include <set>
#include <unordered_map>

#include "int.hpp"
#include "ecommon.hpp"
#include "logger.hpp"

class System {
public:
	std::set <EntityHandle> m_entities;
};

class SystemManager {
private:
	std::unordered_map <const char*, Signature> m_signatures;
	std::unordered_map <const char*, std::shared_ptr<System>> m_systems;
public:
	template <typename T>
	std::shared_ptr<T> register_system() {
		const char* type_name = typeid(T).name();

		if (m_systems.find(type_name) != m_systems.end()) {
			log(LOG_WARNING, "System already registered");
			return (std::static_pointer_cast<T>(m_systems[type_name]));
		}

		auto sys = std::make_shared<T>();
		m_systems.insert({type_name, sys});
		return sys;
	}

	template <typename T>
	void set_signature(Signature signature) {
		const char* type_name = typeid(T).name();

		if (m_systems.find(type_name) == m_systems.end()) {
			log(LOG_WARNING, "System not registered.");
			register_system<T>();
		}

		m_signatures.insert({type_name, signature});
	}

	void on_entity_destroy(EntityHandle entity);
	void on_entity_signature_changed(EntityHandle entity, Signature signature);
};
