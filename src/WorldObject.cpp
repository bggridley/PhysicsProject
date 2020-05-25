
#include "WorldObject.h"

class World;

using namespace Magnum;
using namespace Math::Literals;

WorldObject::WorldObject(World* w, const Color4& color, Trade::MeshData2D meshData) : world(w), Object2D{ w->scene }, SceneGraph::Drawable2D{ *this, w->drawables }, _parent(w->scene), _color{ color } {
	id = world->count + 1;
	world->count++;
	//Debug{} << "creating world object with id: " << id;

	GL::Buffer vertexBuffer;

	vertexBuffer.setData(meshData.positions(0), GL::BufferUsage::StaticDraw);
	_mesh.setPrimitive(meshData.primitive())
		.setCount(meshData.positions(0).size())
		.addVertexBuffer(std::move(vertexBuffer), 0, Shaders::Flat2D::Position{});

	setPosition({ x, y });
}

// change to setVelocity
void WorldObject::launch(Vector2 vector2) {
	vx = vector2.x();
	vy = vector2.y();
}

void WorldObject::tick(float duration) {
	tickFade(); // could make this simpler and just haev it for all worldObjects (by calling WorldObject::tick(duration) from Derived class,
	_color = Color4{_color.r(), _color.g(), _color.b(), alpha};

	if (tag == 101 && alpha <= 0.0f) {
		return;
	} // skip tick if particles are bad

	if (!world->ended && tag == 6969) { // tag 6969 is the world objects
		setAlpha(health / 3.0f);
	}

	// 90 is the height in meters (pixels to meters conversion)
	if (gravity) {

		vy += (duration) * (1 / 60.0f) * ay * (900.0f / 50.0f); //+= -9.8f * duration / (60.0f) * (50.0 / 900.0f);
		vx += (duration) * (1 / 60.0f) * ax * (900.0f / 50.0f);
		// 900 pixels / 50 meters
		//9.8 pixels per second accel
	}

	x += vx;
	y += vy;

	if (collisions) {
		for (Object2D& obj : world->scene->children())
		{
			if (WorldObject* wobj = dynamic_cast<WorldObject*>(&obj)) {

				if (id != wobj->getId() && tag == 1 && wobj->collisions) { // tag == 1 basically means that only the shootable object will collide

					// cehck if boxes overlap
					if (collidesWith(wobj)) {
							//collisionEvent(wobj);
							//wobj->collisionEvent(this);
							if (wobj->tag != 2 && wobj->tag != 99) { // 2 = basket
								wobj->health--;

								if (wobj->health <= 0) {
									continue; // continue to the next object
								}
							}

							float wy = (2 * (scale.x() + wobj->scale.x())) * (y - wobj->y); // minkowski SUMS
							float hx = (2 * (scale.y() + wobj->scale.y())) * (x - wobj->x);
							if (wy > hx) {
								if (wy > -hx) {
									/* top */
									world->createParticleEffect(Vector2{ x, y - scale.y() + (wobj->getScale().y() / 3.0f)}, Vector2{ 0, 1 }.normalized(), {scale.x() / 2, wobj->getScale().y() / 5.0f}, wobj->getColor(), 11, -vy / 3, 120);
									bounceY(wobj->getBounceFactor());
									y = wobj->y + wobj->scale.y() + scale.y();

									collideTop(wobj);
								}
								else {
								// left side
									world->createParticleEffect(Vector2{ x + scale.x() - (wobj->getScale().x() / 3.0f), y}, Vector2{ -1, 0 }.normalized(), { wobj->getScale().x() / 5.0f, scale.y() / 2}, wobj->getColor(), 11, vx / 3, 120);
									bounceX(wobj->getBounceFactor());
									x = wobj->x - wobj->scale.x() - scale.x();
								}
								/* left */
							}
							else {
								if (wy > -hx) {
									// right
									world->createParticleEffect(Vector2{ x - scale.x() + (wobj->getScale().x() / 3.0f), y }, Vector2{ 1, 0 }.normalized(), { wobj->getScale().x() / 5.0f, scale.y() / 2 }, wobj->getColor(), 11, -vx / 3, 120);
									bounceX(wobj->getBounceFactor());
									x = wobj->x + wobj->scale.x() + scale.x();
								}
								else {
									// bottom
									world->createParticleEffect(Vector2{ x, y + scale.y() - (wobj->getScale().y() / 3.0f) }, Vector2{ 0, -1 }.normalized(), { scale.x() / 2, wobj->getScale().y() / 5.0f }, wobj->getColor(), 11, vy / 3, 120);
									bounceY(wobj->getBounceFactor());
									y = wobj->y - wobj->scale.y() - scale.y();
								}
							}
						
					}
				}
			}
		}
	}


	if (y < scale.y()) {
		bounceY(0.9f);
		y = scale.y();
	}

	if (x < scale.x()) {
		bounceX(0.9f);
		x = scale.x();
	}

	//translate({ vx, vy });

	if (gravity) {
		setPosition({ x, y });
	}
	//setTransformation(Matrix3::translation({ x, y }) * Matrix3::scaling(scale) * Matrix3::rotation(rotation));
}

bool WorldObject::collidesWith(WorldObject* wobj) {
	if (id == wobj->getId()) return false;

	return(x + scale.x() > wobj->x - wobj->scale.x() && x - scale.x() && x - scale.x() < wobj->x + wobj->scale.x() && y + scale.y() > wobj->y - wobj->scale.y() && y - scale.y() < wobj->y + wobj->scale.y());
}

void WorldObject::tickFade() {
	if (fading == 0 || fading == 1) {
		alpha += alphaPerTick;
		if (alpha >= 1.0f) {
			alpha = 1.0f;
			fading = -1;
		}
		else if (alpha <= 0.0f) {
			alpha = 0.0f;
			fading = -1;
		}
	}
}

void WorldObject::collideTop(WorldObject* wobj) {
	if (world->launches <= 0 && vy < 0.1f && vx == 0.0f && world->frames - world->launchFrames >= 60) { // greater than 1 seconds // add friction and this will fix a lot of things
		world->endGame(0);
	}


	if (wobj->tag == 2) {
		wobj->health = 3; // max health
		Debug{} << "you win hehe";
		world->endGame(1);

	}
}


void WorldObject::setColor(Color4 color) {
	_color = color;
}

void WorldObject::draw(const Matrix3& transformationMatrix, SceneGraph::Camera2D& camera) {
	if (tag == 101 && alpha <= 0.0f) {
		return;
	}
		_shader
			.setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix)
			.setColor(_color);
		_mesh.draw(_shader);
}

// 0 = fade out, 1 = fade in

void WorldObject::bounceX(float factor) { // also apply friction in the opposite direction
	vx *= -(bounceFactor * factor);
	vy *= 0.98f;
}

void WorldObject::bounceY(float factor) {
	vy *= -(bounceFactor * factor);
	vx *= 0.98f;
}

float WorldObject::getBounceFactor() {
	return bounceFactor;
}

void WorldObject::setBounceFactor(float bounce) {
	bounceFactor = bounce;
}

void WorldObject::fadeIn(int frames) {
	fading = 1;
	alphaPerTick = (1.0f - alpha) / frames;
}

void WorldObject::fadeOut(int frames) {
	fading = 0;
	alphaPerTick = -alpha / frames;
}

void WorldObject::setAlpha(float alphaa) {
	alpha = alphaa;
	fading = -1;
}

Vector2 WorldObject::getPosition() {
	return Vector2({ x, y });
}

void WorldObject::setPosition(Vector2 pos) {
	x = pos.x();
	y = pos.y();
	setTransformation(Matrix3::translation({ x, y }) * Matrix3::scaling(scale) * Matrix3::rotation(rotation)); //* Matrix3::rotation(45.0_degf));
}

void WorldObject::setVelocity(Vector2 v) {
	vx = v.x();
	vy = v.y();
}

Vector2 WorldObject::getVelocity() {
	return Vector2({ vx, vy });
}


void WorldObject::setAcceleration(Vector2 a) {
	ax = a.x();
	ay = -9.8f + a.y();
}

Vector2 WorldObject::getAcceleration() {
	return Vector2({ ax, ay });
}

void WorldObject::setScale(Vector2 scale) {
	this->scale = scale;
	setTransformation(Matrix3::translation({ x, y }) * Matrix3::scaling(scale) * Matrix3::rotation(rotation)); //* Matrix3::rotation(45.0_degf));
}

Vector2 WorldObject::getScale() {
	return scale;
}

void WorldObject::setRotation(Deg degs) {
	this->rotation = degs;
	setTransformation(Matrix3::translation({ x, y }) * Matrix3::scaling(scale) * Matrix3::rotation(rotation)); //* Matrix3::rotation(45.0_degf));
}

void WorldObject::setGravity(bool grav) {
	this->gravity = grav;
}

bool WorldObject::getGravity() {
	return gravity;
}

int WorldObject::getHealth() {
	return health;
}

void WorldObject::setHealth(int h) {
	health = h;
}

void WorldObject::setCollisions(bool col) {
	this->collisions = col;
}

int WorldObject::getId() {
	return id;
}

Color4 WorldObject::getColor() {
	return _color;
}

float WorldObject::getMass() {
	return mass;
}

