#pragma once
#include "World.h";
#include "Task.h"
#include <string>
#include <vector>


using namespace Magnum;

class Packet {
public:
	Packet() { }
	virtual void execute(World* world) {

	}
	virtual std::string getString() {
		return "nullpacket";
	}

	void setArgs(std::vector <std::string> a) {
		args = a;
	}

	std::vector<std::string> args;
protected:
	int id;
	// execute the packet (aka create the worldobject, send the message, etc..)

};