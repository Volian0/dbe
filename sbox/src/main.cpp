#include <engine.hpp>

#include <string>

struct TestComponent {
	float x, y, z;
};

struct AnotherTestComponent {
	std::string a;
};

class TestSystem : public System {
public:
	void update(ECS& ecs, double timestep) {
		for (const auto& entity : m_entities) {
			auto& tc = ecs.get_component<TestComponent>(entity);
			auto& atc = ecs.get_component<AnotherTestComponent>(entity);

			printf("timestep: %g\n", timestep);
			printf("TC: %g, %g, %g\n", tc.x, tc.y, tc.z);
			printf("ATC: %s\n", atc.a.c_str());
		}
	}
};

class Sandbox : public Application {
private:
	ECS ecs;

	std::shared_ptr<TestSystem> ts;
public:
	void on_init() override {
		ecs.register_component<TestComponent>();
		ecs.register_component<AnotherTestComponent>();

		{
			ts = ecs.register_system<TestSystem>();
			Signature sig;
			sig.set(ecs.get_component_type<TestComponent>());
			sig.set(ecs.get_component_type<AnotherTestComponent>());
			ecs.set_system_signature<TestSystem>(sig);
		}

		EntityHandle e = ecs.new_entity();
		ecs.add_component<TestComponent>(e, {0.3, 4.0, 3.0});
		ecs.add_component<AnotherTestComponent>(e, {"Hello, world"});
	}

	void on_update() override {
		ts->update(ecs, m_window->timestep);
	}

	void on_destroy() override {

	}
};

int main() {
	Sandbox sbox;
	sbox.run({
		640, 480,
		"Sandbox",
		false
	});
}
