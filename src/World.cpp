#pragma once
#include "World.h"
#include "WorldObject.h"
#include "TextObject.h"
#include <Corrade/Utility/FormatStl.h>

using namespace Magnum;
using namespace Math::Literals;

/*
tags:
1 = player
6969 = game object that can be collided with (random objects)
99 = planet
100 = win/lose text
101 = particle effect
102 = launch area
*/

	World::World(Object2D* s, SceneGraph::DrawableGroup2D* d) : scene(s), drawables(d) {
		for (int i = 0; i < 30; i++) { // 100 reserved particles
			WorldObject* particle = new WorldObject(this, 0xffffff_rgbf, Primitives::squareSolid());
			particle->setCollisions(false);
			particle->alpha = 0.0f;
			particle->tag = 101; // tag 101 = particle
			particle->setScale({ 0, 0 });
			particle->setPosition({ 0, 0 });
		}
	};

	void World::endGame(int state) {
		launches = 10;
		if (!ended) {
			endFrames = frames;
			obj->setPosition({ 200, 200 });
			obj->setVelocity({ 0, 0 });
			std::string msg = state == 1 ? "You win!" : "You lose!";
			gameText->updateText(Utility::formatString(msg.c_str()));
			gameText->fadeIn(60);
			for (Object2D& objj : scene->children())
			{
				if (WorldObject* randobj = dynamic_cast<WorldObject*>(&objj)) {
					if (randobj->tag != 100) {
						randobj->fadeOut(60); // again, why this called randmo object if they're nto all obj??
					}
					if (randobj->tag == 6969) {
						randobj->setGravity(true);
						randobj->setCollisions(false);
					}
				}
			}


			ended = true;
		}
	}

	void World::tick(int fps) {
		for (Packet* packet : packets) {
			packet->execute(this);
		}

		packets.clear(); // maybe use an iterator and erase so that if an item is added midway through iteration the packet isn't missed

		if (frames - endFrames == fps * 2 && ended) { // 3 seconds
			setResettables(0.0f);

			Debug{} << "reset world";
		}
		if (frames - endFrames == fps * 3 && ended) {
			endFrames = 0;
			ended = false;
		} // make it fully ended after 3 seconds to compensate for the fade, so that all of the objects are not instantly set to alpha = 1.0f
	}
	void World::setResettables(float alpha) {
		std::random_device rd;    
		std::mt19937 rng(rd());    
		std::uniform_int_distribution<int> xrand(75, 2500);
		std::uniform_int_distribution<int> yrand(75, 2000); 
		std::uniform_int_distribution<int> scalerand(16, 75); 
		std::uniform_int_distribution<int> color(0, 100);

		// ls = launch site
		std::uniform_int_distribution<int> lspos(400, 650); 
		std::uniform_int_distribution<int> lsscale(150, 250); 

		std::uniform_int_distribution<int> basketX(2250, 2500);
		std::uniform_int_distribution<int> basketY(300, 1500);
	
		basket->setPosition(Vector2({ basketX(rng) * 1.0f, basketY(rng) * 1.0f }));

		launchArea->setPosition(Vector2({lspos(rng) - 100.0f * 1.0f, lspos(rng) * 1.0f}));
		launchArea->setScale(Vector2({lsscale(rng) * 1.0f, lsscale(rng) * 1.0f}));
		// here we fix the collisions

		for (Object2D& objj : scene->children())
		{
			if (WorldObject* randobj = dynamic_cast<WorldObject*>(&objj)) {
				if (randobj->tag != 100 && randobj->tag != 101) { // not 100 or 101 which is particles
					randobj->fadeIn(60); // why this called randobj if they're nto all random>???
				} /// fade it as long as it's not the win text
				if (randobj->tag == 6969) {
				loop:

					int x = xrand(rng);
					int y = yrand(rng);
					randobj->setPosition({ x * 1.0f, y * 1.0f });
					randobj->setScale({ scalerand(rng) * 1.0f, scalerand(rng) * 1.0f });

					Debug{} << "setting pos to: " << randobj->getPosition();

					for (Object2D& objj1 : scene->children())
					{
						if (WorldObject* wobj = dynamic_cast<WorldObject*>(&objj1)) {
							if ((wobj->tag == 6969 || wobj->tag == 2 || wobj->tag == 102) && randobj != wobj && randobj->collidesWith(wobj)) {
								goto loop;
							}
						}
					}

					
						
						randobj->setGravity(false);
						randobj->setVelocity({ 0.0f, 0.0f }); // make sure the V from the last frame does not carry over

						randobj->setColor(Color4{ color(rng) / 100.0f, color(rng) / 100.0f, color(rng) / 100.0f, alpha });
						randobj->setCollisions(true);
						randobj->setHealth(3);

					}
				}
			}


		if (alpha == 0.0f) { // kind of a hack, but means it was called from this class
			gameText->fadeOut(60);
		}
	}

	void World::createParticleEffect(Vector2 position, Vector2 direction, Vector2 scale, Color4 color, int count, float speed, int duration) {
		int created = 0;
		for (Object2D& obj : scene->children()) {
			if (WorldObject* particle = dynamic_cast<WorldObject*>(&obj)) {
				if (particle->alpha <= 0.0f) {
					if (particle->tag == 101) {
						particle->alpha = 1.0f;
						std::random_device rd;     // only used once to initialise (seed) engine
						std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
						std::uniform_int_distribution<int> xrand(-100, 100); // guaranteed unbiased
						std::uniform_int_distribution<int> yrand(-100, 100); // guaranteed unbiased

						particle->setGravity(true);
						particle->setPosition(position);
						particle->setScale(scale);
						particle->setVelocity((direction * speed) + Vector2({ xrand(rng) / 75.0f, yrand(rng) / 75.0f}));
						particle->fadeOut(60);
						particle->setColor(color);

						if (created < count) {
							created++;
						}
						else {
							break;
						}
					}
				}
			}
		}
	}

	int World::getRandom(int start, int end) {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> rand(start, end);
		return rand(rng);
	}

	void World::createObject(World* world) {
		new WorldObject(world, 0xffffff_rgbf, Primitives::squareSolid());
	}