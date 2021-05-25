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
	Scene scene;

	std::shared_ptr<TestSystem> ts;
public:
	void on_init() override {
		scene.ecs.register_component<TestComponent>();
		scene.ecs.register_component<AnotherTestComponent>();

		{
			ts = scene.ecs.register_system<TestSystem>();
			Signature sig;
			sig.set(scene.ecs.get_component_type<TestComponent>());
			sig.set(scene.ecs.get_component_type<AnotherTestComponent>());
			scene.ecs.set_system_signature<TestSystem>(sig);
		}

		Entity e = scene.new_entity();
		e.add_component<TestComponent>({0.3, 4.0, 3.0});
		e.add_component<AnotherTestComponent>({"Hello, world"});
	}

	void on_update() override {
		ts->update(scene.ecs, m_window->timestep);
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
