#pragma once
#include "WorldObject.h"
#include <Magnum/Primitives/Line.h>
#include "Types.h"
using namespace Magnum;
using namespace Math::Literals;

class TriangleObject : public WorldObject {
public:
	TriangleObject(World* world, const Color4& color) : WorldObject(world, color, Primitives::line2D()) {
		std::vector<Vector2> positions;

		positions.push_back(Vector2({-1.0f, -1.0f}));
		positions.push_back(Vector2({0, -1.0f}));
		positions.push_back(Vector2({0, 0}));

		GL::Buffer vertexBuffer;
		vertexBuffer.setData(std::move(positions), GL::BufferUsage::StaticDraw);

		/* Configure the mesh, add vertex buffer */
		_mesh.setPrimitive(MeshPrimitive::LineLoop)
			.addVertexBuffer(std::move(vertexBuffer), 0, Shaders::Flat2D::Position{})
			.setCount(positions.size());
	}
};