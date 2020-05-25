#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h> // TCP winsock
#include <thread>

#pragma comment (lib, "ws2_32.lib")
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Containers/Pointer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Complex.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Primitives/Line.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Timeline.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Audio/AbstractImporter.h>
#include <Magnum/Audio/Buffer.h>
#include <Magnum/Audio/Context.h>
#include <Magnum/Audio/Listener.h>
#include <Magnum/Audio/Playable.h>
#include <Magnum/Audio/PlayableGroup.h>
#include <Magnum/Audio/Source.h>
#include "WorldObject.h"
#include "TextObject.h"
#include "Types.h"
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include "WorldObject.h"
#include "World.h"
#include "ParabolaObject.h"
#include "TriangleObject.h"
#include "PlanetObject.h"
#include <random>
#include "PacketHandler.h"
#include "Packet.h"
#include "Packet0Connect.h"
#include "Task.h"


using namespace Magnum;
using namespace Math::Literals;

class PhysicsEngine : public Platform::Application {
public:
	~PhysicsEngine();
	explicit PhysicsEngine(const Arguments& arguments);
	Magnum::Matrix3 defaultProjectionMatrix();
private:
	friend class World;
	World* world;
	Vector2 point1;
	Vector2 mousePos;
	PluginManager::Manager<Text::AbstractFont> _fontManager;
	PluginManager::Manager<Audio::AbstractImporter> _audioManager;
	bool pressed = false;
	Vector2 unproject(const Vector2i& windowPosition) const;
	void updateVisualPositions();
	void mousePressEvent(MouseEvent& e);
	void mouseReleaseEvent(MouseEvent& e);
	void mouseMoveEvent(MouseMoveEvent& e);
	void keyPressEvent(KeyEvent& e);
	void keyReleaseEvent(KeyEvent& e);
	void viewportEvent(ViewportEvent& e);
	void updateWindow();
	void drawEvent();
	void tickEvent();
	float zoom = 1.0f;

	Scene2D _scene;
	Object2D* _cameraObject;


	SceneGraph::Camera2D* _camera;
	SceneGraph::DrawableGroup2D _drawables;

	// FONT STUFF
	Containers::Pointer<Text::AbstractFont> _font;
	Text::DistanceFieldGlyphCache _cache;

	// AUDIO STUFF

	Containers::Pointer<Audio::AbstractImporter> _importer;
	Audio::Context _context;
	

	// PLAYABLES

	Audio::Listener2D _listener;
	Audio::PlayableGroup2D _playables;

	Timeline timeline;

	int ol = 0; // why the fuck did i name this ol?
	WorldObject* obj;

	TriangleObject* launchObject;

	WorldObject* launchArea;

	TextObject* text;
	TextObject* gameText;
	TextObject* dynamicText;

	ParabolaObject* parabola;
	PlanetObject* planet;

	WorldObject* basket;
	WorldObject* axisX;
	WorldObject* axisY;

	Vector2 launch;
	float launchX = 0;
	float launchY = 0;
	float launchHeight;
	float fps = 60;

	int ticks = 0;
	int frames = 0;
	int timeCounter = 0;
	int totalFrames = 0;

	float totalTime = 0; // this set so the text can be changed from other places
	float timeTo = 0; // ACTUAL time, decremented by actual time
	float worldScale;

	float windowX = 800;
	float windowY = 600;
	bool zoomIn = false;
	bool zoomOut = false;
	bool focus = false;

	SOCKET sock;
	void sendPacket(Packet* packet);
	std::thread thread_object;
	void listen();
	void network();
	PacketHandler* packetHandler;

	// 900 / 50 means 50 meters
			//World* world;
};