#pragma once
#include <string>
#include "Packet.h"
#include "Packet0Connect.h"
#include "World.h"
#include "Task.h"
#include <vector>

using namespace Magnum;

class PacketHandler {
public:
	struct PacketFactory { virtual Packet* create() = 0; };

	template<typename Type> struct Factory : public PacketFactory {
		virtual Type* create() {
			return new Type();
		}
	};

	std::vector<PacketFactory*> packetList;

	PacketHandler(World* w) : world(w) {
		packetList.push_back(new Factory<Packet0Connect>);
	}

	Packet* createAndExecute(char buf[4096]) {
	
		std::string s = std::string(buf);

		std::vector<std::string> args;

		std::string tmp;
		std::stringstream ss(s);

		while (getline(ss, tmp, ',')) {
			args.push_back(tmp);
		}

		int id = std::stoi(args[0]);
		std::cout << args[1]; // better work
		Packet* packet = packetList[id]->create();
		packet->setArgs(args);
		world->packets.push_back(packet);

		// pray amd hope
		//packet->execute(world, args);

		// instead of calling execute here, somehow find a wayy to create a list of functions / code chunks that will be executed later on the main thread (basiaclly on the next tick)
		// so that the GL context is preserved
		return packet;
	}


private:
	World* world;
};