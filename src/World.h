#pragma once
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include "Types.h"
#include <Magnum/Trade/MeshData2D.h>
#include "Packet.h"
#include <random>

// working: include WorldObject.h, class WorldObject

using namespace Magnum;
using namespace Math::Literals;

class TextObject;
class WorldObject;
class Packet;
// to add more just include more stuff yurd meh?

class World {
public:
	World(Object2D* s, SceneGraph::DrawableGroup2D* d);
	std::string clientName;
	int count = 0;
	Object2D* scene;
	SceneGraph::DrawableGroup2D* drawables;
	std::vector<Packet*> packets;
	float scale;
	int frames;
	int launchFrames;
	bool ended = false;
	std::vector<std::string> strings;
	int launches = 10;

	// vector
	//    - duration, vector of particles
	//    - duration, vector of particles 
	void createParticleEffect(Vector2 position, Vector2 direction, Vector2 scale, Color4 color, int count, float speed, int duration); // velocity is a float here because the direction will be multiplied by this to create randomness

	WorldObject* launchArea;
	WorldObject* launchObject;
	WorldObject* obj;
	WorldObject* basket;
	TextObject* gameText;

	void endGame(int state); // 0 = lose, 1 = win

	void tick(int fps);
	void setResettables(float alpha);
	int endFrames = 0;
	int getRandom(int start, int end);

	void createObject(World* world);
};