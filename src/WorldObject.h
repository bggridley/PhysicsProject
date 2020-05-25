#pragma once
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData2D.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Primitives/Line.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Extensions.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Corrade/Containers/LinkedList.h>
#include <Corrade/Containers/Array.h>
#include <Magnum/Platform/Sdl2Application.h>
#include "Types.h"
#include "World.h"
using namespace Magnum;
using namespace Math::Literals;

class World;

// working : include World.j, class World

class WorldObject : public Object2D, public SceneGraph::Drawable2D {

public:
	WorldObject(World* w, const Color4& color, Trade::MeshData2D meshData);
	int getId();
	Vector2 getPosition();
	Vector2 getVelocity();
	Vector2 getAcceleration();
	Vector2 getScale();
	Color4 getColor();
	void setCollisions(bool col);
	virtual void setColor(Color4 color);
	void setPosition(Vector2 pos);
	void setVelocity(Vector2 v);
	void setAcceleration(Vector2 a);
	void setRotation(Deg degs);
	void setGravity(bool grav);
	bool getGravity();
	void setHealth(int health);
	int getHealth();
	virtual void tick(float duration);
	virtual void draw(const Matrix3& transformationMatrix, SceneGraph::Camera2D& camera);
	void setScale(Vector2 scale);
	void launch(Vector2 vector);
	float getMass();
	float alpha = 1.0f;
	void setAlpha(float alpha); // purpose is to reset the fade
	void fadeIn(int frames);
	void fadeOut(int frames);
	void setBounceFactor(float bounce);
	float getBounceFactor();
	bool collidesWith(WorldObject* wobj);
	int tag = -1;
	std::string multiplayer_name;
protected:
	int health = 3; // 3 total strikes and then it disappears
	int fading = -1; // 0 = fade TO BALCK, 1 = FADE FROM BLACK
	float alphaPerTick = 0.0f;
	Shaders::Flat2D _shader;
	Color4 _color;
	Object2D* _parent;
	Vector2 scale{18, 18};
	Deg rotation;
	int id = 0;
	float x = 0;
	float y = 0;
	float vx = 0;
	float vy = 0;
	float ax = 0;
	float ay = -9.8f;

	// NOTE AX and AY are in METERS, while the others are in PIXELs
	float mass = 200; // in grams;
	float bounceFactor = 0.7f;
	bool gravity = false;
	bool collisions = false;
	void bounceX(float factor);
	void bounceY(float factor); // factor here is the other object's bounce factor
	void tickFade();
	World* world;
	GL::Mesh _mesh;
private:
	void collideTop(WorldObject* wobj);
};