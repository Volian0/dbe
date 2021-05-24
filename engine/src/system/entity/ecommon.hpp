#pragma once

#include <bitset>

#include "int.hpp"

typedef u32 EntityHandle;
const u32 MAX_ENTITIES = 100;

typedef u32 ComponentType;
const u32 MAX_COMPONENTS = 32;

typedef std::bitset<MAX_COMPONENTS> Signature;
