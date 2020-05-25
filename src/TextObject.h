#pragma once
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Math/Matrix3.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/Pointer.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Renderer.h>
#include "WorldObject.h"
#include "World.h"
#include "Types.h"
#include <Magnum/Primitives/Line.h>

using namespace Magnum;

class World;

class TextObject : public WorldObject {
public:
	TextObject(World* w, Matrix3 defaultProjectionMatrix, PluginManager::Manager<Text::AbstractFont>& manager, Containers::Pointer<Text::AbstractFont>& font, Text::DistanceFieldGlyphCache& cache, const Color4& color, Text::Alignment align, float size) : WorldObject(w, color, Primitives::line2D()), _cache(cache), _defaultProjectionMatrix(defaultProjectionMatrix)
	{
		_dynamicText.reset(new Text::Renderer2D(*font, cache, size, align));
		_dynamicText->reserve(150, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);
		GL::Renderer::enable(GL::Renderer::Feature::Blending);
		GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, GL::Renderer::BlendFunction::OneMinusSourceAlpha);
		GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
		setPosition({0, 0});
		setScale({ 1.0f, 1.0f });
	}

	void updateText(std::string text);
	void draw(const Matrix3& transformationMatrix, SceneGraph::Camera2D& camera) override;
	void tick(float duration) override;
	World* world;
private:
	Matrix3 _defaultProjectionMatrix;
	Containers::Pointer<Text::Renderer2D> _dynamicText;
	Shaders::DistanceFieldVector2D _shader;
	Text::DistanceFieldGlyphCache& _cache;
	//void setScale(Vector2 scale);
};