#include <engine.hpp>

class Sandbox : public Application {
private:
	Scene scene;
public:
	void on_init() override {

	}

	void on_update() override {

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
