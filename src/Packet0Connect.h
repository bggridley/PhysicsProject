#pragma once
#include "Packet.h"
#include "World.h"
#include "WorldObject.h"
#include <sstream>
#include <iostream>

class Packet0Connect : public Packet {
	// packetID, socket(ofconnectingobject);

public:

	Packet0Connect() {
		// do nothin
	}

	Packet0Connect(std::string n) {
		name = n;
		id = 0;
	}

	std::string getString() override {
		std::ostringstream o;
		std::string s;
		o << id << "," << name; // maybe change this to UUID's or somethign of the sort in the future
		s = o.str();

		//yessur

		return s;
	}

	// this works because it is called from the main thread oh yeah oh yeah
	void execute(World* world) override { // do the arguments yurd meh.  args[0] = id, args[1] = etc..
		//if (std::stoi(args[1]) != world->clientSocket) {
		if (args.at(1) != world->clientName) { 
			WorldObject* tempObject = new WorldObject(world, 0xffffff_rgbf, Primitives::squareSolid());
			tempObject->setPosition({ 200, 200 });
			tempObject->multiplayer_name = name; // unique socket identifiers for each object 
		}


		//}
	}

private:
	std::string name;
};