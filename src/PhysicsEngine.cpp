#include "PhysicsEngine.h"
#include <SDL.h>
// winsock


//#include "World.h"
#include <iostream>

	using namespace Magnum;
	using namespace Math::Literals;


	PhysicsEngine::~PhysicsEngine() {
		Debug{} << "is this actually called?";
		closesocket(sock);

		WSACleanup();

		//thread_object.join(); // handle the thread being ended somehow
	}

	void PhysicsEngine::network() {
		using namespace std;
		string ip; // same thing as localhost
		int port = 25567;

		cout << "Enter IP: " << endl;

		getline(cin, ip);

		WSADATA wsData;
		WORD version = MAKEWORD(2, 2);

		int wsStart = WSAStartup(version, &wsData);
		if (wsStart != 0) {
			std::cerr << "Failed to start winsock" << endl;
			return;
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
			cerr << "Cannot create socket" << endl;
			WSACleanup();
			return;
		}

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port); // haha minecraft port
		inet_pton(AF_INET, ip.c_str(), &hint.sin_addr); // INET = IPv4 which means smaller data

		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
		if (connResult == SOCKET_ERROR) {
			cerr << "Failed to connect to server" << endl;
			closesocket(sock);
			WSACleanup();
		}


		cout << "Connected! With socket id: " << sock << endl;

		std::string name;

		std::cout << "Enter your name: " << std::endl;
		getline(std::cin, name);

		world->clientName = name;

		sendPacket(new Packet0Connect(name));

		thread_object = std::thread(&PhysicsEngine::listen, this);
	}

	void PhysicsEngine::listen() {
		using namespace std;
		char buf[4096];
		string clientInput;

		while (true) {
			int bytesReceived = recv(sock, buf, 4096, 0);
			if (bytesReceived > 0) {
				cout << string(buf, 0, bytesReceived) << endl;
				packetHandler->createAndExecute(buf);
				// gonna throw errors cus the server appends random shit to the end of it
			}
		}
	}

	void PhysicsEngine::sendPacket(Packet* packet) {
		std::string pstring = packet->getString();
		int result = send(sock, pstring.c_str(), pstring.size(), 0);

		if (result == SOCKET_ERROR) {
			Debug{} << "socket error!";
		}

		delete packet;
	}


	// 900 PIXElS = 50 meters
	PhysicsEngine::PhysicsEngine(const Arguments& arguments) :
		Platform::Application{ arguments, Configuration{}
			.setTitle("Physics").setWindowFlags(Configuration::WindowFlag::AllowHighDpi).setSize({800, 600}, Configuration::DpiScalingPolicy::Virtual), GLConfiguration{}.setSampleCount(4) }, _context{ Audio::Context::Configuration{}
			.setHrtf(Audio::Context::Configuration::Hrtf::Enabled),
		arguments.argc, arguments.argv}, 
		
		_cache{ Vector2i{2048}, Vector2i{512}, 22 }, _listener{_scene}
	{
		_cameraObject = new Object2D{ &_scene };
		_camera = new SceneGraph::Camera2D{ *_cameraObject };
		updateWindow();
		Utility::Resource rs("res");

		_font = _fontManager.loadAndInstantiate("StbTrueTypeFont");
		if (!_font || !_font->openData(rs.getRaw("reitam.otf"), 180.0f))
			Fatal{} << "Cannot open font file";

		_font->fillGlyphCache(_cache,
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"01234567890,:;-!<>. ");

		Containers::Pointer<Audio::AbstractImporter> importer = _audioManager.loadAndInstantiate("StbVorbisAudioImporter");
		if (!importer)
			std::exit(1);

		if (!importer->openData(rs.getRaw("bounce.ogg")))
			std::exit(2);

		// create le thread

		// init packet handler

		

		world = new World(&_scene, &_drawables);
		world->scale = 50.0f;
		worldScale = world->scale;

		//auto square = new Object2D(&_scene); ID = 1
		obj = new WorldObject(world, 0xffffff_rgbf, Primitives::squareSolid());
		obj->setCollisions(true);
		obj->setPosition({ 500, 425 }); 
		obj->setGravity(true);
		obj->tag = 1;

		world->obj = obj; // assign obj in world to a pointer to obj

		basket = new WorldObject(world, 0x00ffff_rgbf, Primitives::squareSolid()); // basket ID = 2
		basket->setCollisions(true);
		basket->setPosition({ 1600, 200 });
		basket->setScale({ 100, 20 });
		basket->tag = 2;
		world->basket = basket;

		launchArea = new WorldObject(world, 0xffff00_rgbf, Primitives::squareWireframe());
		launchArea->tag = 102;
		world->launchArea = launchArea;
		// should really move a lot of these into world unless they need to be accessed from this class

		parabola = new ParabolaObject(world, 0xff00ff_rgbf, 30);
		parabola->setScale({0, 0});

		axisX = new WorldObject(world, 0xffff00_rgbf, Primitives::line2D());
		axisX->setScale({ windowX * 1.5f / zoom, 10.0f});
		axisY = new WorldObject(world, 0xffff00_rgbf, Primitives::line2D());
		axisY->setRotation(90.0_degf);
		axisY->setScale({ 10.0f, windowY * 1.5f / zoom});
		planet = new PlanetObject(world, 0xe7c5fc_rgbf);
		planet->setPosition({ 10000, 10000 });
		planet->setScale({ 1000, 1000});

	
		for (int i = 0; i < 50; i++) {
		
				WorldObject* objj = new WorldObject(world, 0xffff00_rgbf, Primitives::squareSolid());
				objj->tag = 6969; // maybe work on a better way of identifying objects, u already have id, but now tag too??
				objj->setCollisions(false);
		}

		// then again, this doesn't even need to be made in this class ^^

		world->setResettables(1.0f); // start with 1.0 alpha
		
		gameText = new TextObject(world, defaultProjectionMatrix(), _fontManager, _font, _cache, 0xffff00_rgbf, Text::Alignment::MiddleCenter, 48.0f);
		gameText->setPosition({ 600, 450 });
		gameText->tag = 100; // 100 tag is the YOU LOSE/WIN TEXT

		gameText->alpha = 0.0f; // fix this in the future
		world->gameText = gameText;

		text = new TextObject(world, defaultProjectionMatrix(), _fontManager, _font, _cache, 0xff0000_rgbf, Text::Alignment::LineLeft, 20.0f);
		text->setPosition({ 0, 880 });

		dynamicText = new TextObject(world, defaultProjectionMatrix(), _fontManager, _font, _cache, 0xff0000_rgbf, Text::Alignment::LineRight, 20.0f);
		dynamicText->setPosition({ 1200, 880 });


		launchObject = new TriangleObject(world, 0xffff00_rgbf);
		world->launchObject = launchObject;

		setMinimalLoopPeriod(1000.0f / fps);
		setSwapInterval(1);
		timeline.start();

		GL::Renderer::enable(GL::Renderer::Feature::Blending);
		GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

		//packetHandler = new PacketHandler(world);
		//network();
	}

	void PhysicsEngine::updateWindow() {
		
		
		if (!focus) {
			_camera->setProjectionMatrix(Matrix3::projection({ windowX * 1.5f / zoom, windowY * 1.5f / zoom }) * Matrix3::translation({ -(windowX * 1.5f / zoom / 2), -(windowY * 1.5f / zoom / 2) }))
				.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
		} else {
			_camera->setProjectionMatrix(Matrix3::projection({ windowX * 1.5f / zoom, windowY * 1.5f / zoom }) * Matrix3::translation(-obj->getPosition() - obj->getScale()))
				.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
		}
	}

	void PhysicsEngine::tickEvent() {


		axisX->setScale({ windowX *1.5f / zoom, 10.0f });
		axisX->setPosition({ 0, 0 });
		
		axisY->setScale({ 10.0f, windowY * 1.5f / zoom });
		axisY->setPosition({ 0, 0 });
		float delta = timeline.previousFrameDuration();
		//obj->setRotation(obj->getPosition().x() * 1.0_degf);
		if (frames % 10 == 0) {
			Color4 color = Color4(world->getRandom(0, 100) / 100.0f, world->getRandom(0, 100) / 100.0f, world->getRandom(0, 100) / 100.0f, 1.0f);
			world->createParticleEffect(obj->getPosition(), obj->getVelocity().normalized() * -1.0f, obj->getScale() / 2, color, 1, 2, 20);
		}

		if (focus) {
			updateWindow();
		}

		if (zoomIn) {
			zoom += 0.1 / 60.0f;

			updateWindow();
		
		}
		else if (zoomOut) {
			zoom -= 0.1 / 60.0f;
			updateWindow();
		}


		if (timeTo <= 0) {
			timeTo = 0;
		}
		else {
			timeTo -= delta;
		}
		
		
		for (Object2D& obj : _scene.children())
		{
			if (WorldObject* wobj = dynamic_cast<WorldObject*>(&obj)) {
				wobj->tick(delta);
			}
		}

		if (frames % 10 == 0) {
			float velY = obj->getVelocity().y() * fps / 900 * worldScale;
			dynamicText->updateText(Utility::formatString("Time To Ground: {0}\n Tries Left: {1}", timeTo, world->launches));
			text->updateText(Utility::formatString("Scale: {2}m x {3}m\nVel: <{0}, {1}>",
				obj->getVelocity().x(), obj->getVelocity().y(), worldScale / zoom * (800.0f / 600.0f), worldScale / zoom)); // 1.333 is 800 / 600
			//Debug{} << obj->getPosition().y();
		}

		//w.x += 1.0 * timeline.previousFrameDuration();

	//	Debug{} << (float) timeline.previousFrameTime() * 1.0f;
	//	Debug{} << timeCounter; * 60.0f
		timeCounter += delta;
		if (timeCounter > 1.0f) {
			timeCounter = 0.0f;
			//Debug{} << "ticks: " << ticks << "frames: " << frames;
			ticks = 0;
			frames = 0;
		}

		updateVisualPositions();
		ticks++;
		world->tick(fps);
	}

	void PhysicsEngine::mousePressEvent(MouseEvent& e) {
		if (frames > 60) { // prevent weird bug at the start
			Vector2 point = unproject(e.position());
		

			if (point.x() > launchArea->getPosition().x() - launchArea->getScale().x() && point.x() < launchArea->getPosition().x() + launchArea->getScale().x()) {
				if (point.y() > launchArea->getPosition().y() - launchArea->getScale().y() && point.y() < launchArea->getPosition().y() + launchArea->getScale().y()) {
					point1 = point;
					launchArea->setAlpha(1.0f);
					launchObject->setAlpha(1.0f);

				// reset launch
				launch = Vector2{ 0, 0 };
				pressed = true;

				e.setAccepted(true);
				}
			}
		}
	}

	void PhysicsEngine::updateVisualPositions() {
		if (!pressed) {
			//obj2->setScale({ 0, 0 });
			//obj3->setScale({ 0, 0 });
			//obj4->setScale({ 0, 0 });
		} else {
			float x = launch.x();
			float y = launch.y();
			Deg angle = Math::atan(y / x);

			if (x >= 0) {
				angle = angle + 180.0_degf;
			}

			Vector2 launch = Vector2{ x, y } / 30.0f;
			float launchX = launch.x() * fps / 900 * worldScale; // velx
			float launchY = launch.y() * fps / 900 * worldScale; // vely
			if (launchY > 0) {
				parabola->setScale({ launchX  * (launchY * 900 / worldScale / 9.8f), (launchY * launchY) / 2.0f / 9.8f * 900 / worldScale});
				// the issue to this being off sometimes HAS to be with the velx and vely stuff... is very weird
				Vector2 parabpos = point1 + Vector2{ parabola->getScale().x(), 0.0f};
				parabola->setPosition(parabpos);
				//Debug{} << parabola->getScale();
				//Debug{} << "the only pLACE where the parabola position is set: " << parabpos;
			}
			else {
				parabola->setScale({ 0, 0 });
			}

			launchObject->setPosition(point1 * 1.0f);
			launchObject->setScale(Vector2{x, y});
		}
	}

	void PhysicsEngine::mouseMoveEvent(Magnum::Platform::Sdl2Application::MouseMoveEvent& e) {
		if (pressed) {
			mousePos = unproject(e.position());
			launch = point1 - mousePos; // this is in PIXELS
		}
		updateVisualPositions();

		//Debug{} << "moouse move: " << parabola->getPosition();
		e.setAccepted(true);
;	}

	void PhysicsEngine::mouseReleaseEvent(MouseEvent& e) {
		if (pressed) {
			pressed = false;
			//mousePos = unproject(e.position());

			launchObject->fadeOut(fps);

			if (world->launches != 0) {
				Vector2 launch = this->launch / 30.0f;

				obj->setPosition(point1);
				obj->launch(launch);

				world->createParticleEffect(point1, obj->getVelocity().normalized() * -1.0f, obj->getScale() / 2, 0xff0000_rgbf, 7, 3, 120);
				world->createParticleEffect(point1, obj->getVelocity().normalized() * -1.0f, obj->getScale() / 2, 0xeb8f34_rgbf, 7, 3, 120);
				world->createParticleEffect(point1, obj->getVelocity().normalized() * -1.0f, obj->getScale() / 2, 0xebd334_rgbf, 7, 3, 120);
				world->launches--;



				float launchX = launch.x() * fps / 900 * worldScale; // velx
				float launchY = launch.y() * fps / 900 * worldScale; // vely
				launchHeight = (point1.y() - obj->getScale().y()) * (worldScale / 900.0f); // convert to meters;


				//float time = Math::sqrt(((obj->getPosition().y() - 20.0f) * (50.0f / 900.0f)) * 2.0f / 9.8f); // TIME IF VELOCITY IS NEGATIVE ( calculate based off of distance)
				//float velY = obj->getVelocity().y() * fps / 900 * 50.0f; // convert to "meters" This is in METERS per FRAME
				//if (velY > 0) { // positive velocity = GOING UPWARD
					//time = (velY * fps / 900 * 50) / 9.8f; // 
					//float maxH = launchHeight + ((launchY * launchY) / (2.0f * 9.8f));
					//Debug{} << maxH << "lHeight:" << launchHeight << "launchY: " << launchY;
					//time += Math::sqrt((maxH) * 2.0f / 9.8f);
				float time = 0;
				if (launchY > 0) {
					time = launchY / 9.8f; // A -> B : vf = vi + at; t = -vi/-9.8
					float maxHeight = launchHeight + ((launchY * launchY) / (2.0 * 9.8f)); // vf^2 = vi^2 + 2ad; d = vi^2/2a
					Debug{} << maxHeight;
					time += Math::sqrt(2.0f * (maxHeight) / 9.8f); // B -> C : d = vit + 1/2at^2; t = sqrt(2d/a);

					//Debug{} << "moouse released: " << parabola->getPosition();
				}
				else {
					// d = vit + 1/2at^2
					// 1/2at^2 + vit - d = 0;
					// qA = 1/2a;
					// qB = vi;
					// qC = -d;
					launchY = Math::abs(launchY);
					time = (-launchY + Math::sqrt((launchY * launchY) - (4 * 4.9 * -launchHeight))) / (2 * 4.9);
					//float qNeg = (-launchY - Math::sqrt((launchY * launchY) - (4 * 4.9 * -launchHeight))) / (2 * 4.9);

					//Debug{} << qPos << "NEG : " << qNeg;
					//time = (Math::sqrt(velY + 2 * 9.8 * launchHeight) - velY) / 9.8;
				}
				// PIXELS per 1/60 of a second

				timeTo = time;
				totalTime = time;
			}


			if (world->launches == 0) {
				world->launchFrames = frames;
			}

			updateVisualPositions();

			e.setAccepted(true);
		}
	}

	Vector2 PhysicsEngine::unproject(const Vector2i& windowPosition) const {
		/* We have to take window size, not framebuffer size, since the position is
		   in window coordinates and the two can be different on HiDPI systems */
		const Vector2i viewSize = windowSize();
		const Vector2i viewPosition{ windowPosition.x(), viewSize.y() - windowPosition.y() - 1 };
		const Vector2 in{ 2 * Vector2{viewPosition} / Vector2{viewSize} -Vector2{1.0f} };

		/*
			Use the following to get global coordinates instead of camera-relative:

			(_cameraObject->absoluteTransformationMatrix()*_camera->projectionMatrix().inverted()).transformPoint(in)
		*/
		return _camera->projectionMatrix().inverted().transformPoint(in);
	}

	void PhysicsEngine::keyPressEvent(KeyEvent& e) {
		if (e.key() == KeyEvent::Key::W) {
			zoomIn = true;
		}
		else if (e.key() == KeyEvent::Key::Q) {
			zoomOut = true;
		//	sendPacket("CLIENT ZOOMED OUT!");

		}

		e.setAccepted(true);
	}

	void PhysicsEngine::keyReleaseEvent(KeyEvent& e) {
		if (e.key() == KeyEvent::Key::W) {
			zoomIn = false;
		} else if (e.key() == KeyEvent::Key::Q) {
			zoomOut = false;
		}
		else if (e.key() == KeyEvent::Key::G) {
			for (Object2D& objj : _scene.children())
			{
				if (WorldObject* randobj = dynamic_cast<WorldObject*>(&objj)) {
					if (randobj->tag == 6969) {
						if (randobj->getGravity()) {
							randobj->setGravity(false);
						}
						else {
							randobj->setGravity(true);
						}
					}
				}
			}
		} else if (e.key() == KeyEvent::Key::F) {
			focus = !focus;
			updateWindow();
		}
		else if (e.key() == KeyEvent::Key::R) {
			world->endGame(0);
		}

	
		e.setAccepted(true);
	}

	void PhysicsEngine::drawEvent() {
		//GL::defaultFramebuffer.clearColor((0xffffff_rgbf);)
		GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
		
		
		//_listener.update( _playables);
		_camera->draw(_drawables);

		swapBuffers();
		redraw();
		timeline.nextFrame();
		frames++;
		totalFrames++;
		world->frames = frames;
	}

	void PhysicsEngine::viewportEvent(ViewportEvent& e) {
		windowX = e.windowSize().x();
		windowY = e.windowSize().y();
		updateWindow();
	}

	Magnum::Matrix3 PhysicsEngine::defaultProjectionMatrix() {
		return Matrix3::projection({ windowX * 1.5f, windowY * 1.5f}) * Matrix3::translation({ -(windowX * 1.5f / 2), -(windowY * 1.5f / 2) });
	}


MAGNUM_APPLICATION_MAIN (PhysicsEngine)