#pragma once
#include "Packet.h"
#include "World.h"
#include "WorldObject.h"
#include <sstream>
#include <iostream>

class Packet2Position : public Packet {
	// packetID, socket(ofconnectingobject);

public:

	Packet2Position() {
		// do nothin
	}

	Packet2Position(float x, float y) {
		id = 2; // two because there will be a 1 so that new clients will see players who are already on the server
		this->x = x;
		this->y = y;
	}

	std::string getString() override {
		std::ostringstream o;
		std::string s;
		o << id << "," << x << y; // maybe change this to UUID's or somethign of the sort in the future
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
		}
		//}
	}

private:
	float x;
	float y;
};