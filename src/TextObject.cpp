#include "TextObject.h"

void TextObject::updateText(std::string text) {
	_dynamicText->render(text);
}

void TextObject::draw(const Matrix3& transformationMatrix, SceneGraph::Camera2D& camera) {
	_shader.bindVectorTexture(_cache.texture());
	_shader
		.setTransformationProjectionMatrix(_defaultProjectionMatrix * transformationMatrix)
		.setColor(_color)
		.setOutlineRange(0.5f, 1.0f);

	_dynamicText->mesh().draw(_shader);
}

void TextObject::tick(float duration) {
	_color = Color4{ _color.r(), _color.g(), _color.b(), alpha };
	tickFade();
}