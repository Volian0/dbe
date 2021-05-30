#pragma once

#include <string>

#include "system/scene.hpp"

Entity load_model(Scene& scene, const std::string& path, const std::string& shader);
