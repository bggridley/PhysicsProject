find_package(Corrade REQUIRED Main Utility Containers)
find_package(SDL2 REQUIRED)
find_package(Magnum REQUIRED Audio GL Shaders Sdl2Application SceneGraph MeshTools Trade Primitives Text )
find_package(MagnumPlugins REQUIRED StbTrueTypeFont StbVorbisAudioImporter)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)
corrade_add_resource(Resources resources.conf)

add_executable(PhysicsEngine PhysicsEngine.cpp World.cpp WorldObject.cpp TextObject.cpp ${Resources})
target_link_libraries(PhysicsEngine PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
	Magnum::Audio
    Magnum::Shaders
    Magnum::SceneGraph
    Magnum::MeshTools
	Magnum::Trade
	Magnum::Primitives
	Magnum::Text
	MagnumPlugins::StbTrueTypeFont
	MagnumPlugins::StbVorbisAudioImporter)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PhysicsEngine)
