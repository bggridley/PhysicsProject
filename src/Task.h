#pragma once
#include "World.h"
#include <string>
#include <functional>

class World;
class Task {



public:
	void(execute) (World*, std::string[]);

	Task() {

	}
};