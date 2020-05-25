#pragma once
#include "WorldObject.h"
#include <Magnum/Primitives/Circle.h>
#include "Types.h"
using namespace Magnum;
using namespace Math::Literals;

class PlanetObject : public WorldObject {
public:
	PlanetObject(World* world, const Color4& color) : WorldObject(world, color, Primitives::circle2DSolid(30)) {
	// do nothing
		collisions = true;
		mass = 1.9e13;
		bounceFactor = 1.0;
		tag = 99;
	}

	void PlanetObject::tick(float duration) override {
		WorldObject::tick(duration);

		for (Object2D& obj : world->scene->children())
		{
			if (WorldObject* wobj = dynamic_cast<WorldObject*>(&obj)) {
				if (wobj->getGravity()) {
					Vector2 posPlanet = Vector2{ x, y };
					Vector2 posObj = wobj->getPosition();

					Vector2 dif = (posPlanet - posObj);
					float dist = (Math::sqrt((dif.x() * dif.x()) + (dif.y() * dif.y()))) * world->scale / 900;
					float accel = 6.67e-11 * mass / (dist * dist);

					if (accel > 30.0f || Math::isNan(accel)) {
						accel = 30.0f; // this had to be capped because it would go insanely fast
					}


					wobj->setAcceleration(dif.normalized() * accel);
				}
			}
		}
		
	}
};