#pragma once
#include "WorldObject.h"
#include <Magnum/Primitives/Line.h>
#include "Types.h"
using namespace Magnum;
using namespace Math::Literals;

class ParabolaObject : public WorldObject {
public:
	ParabolaObject(World* world, const Color4& color, int vertices) : WorldObject(world, color, Primitives::line2D()) { // line2d means nothing here
		std::vector<Vector2> positions;

		for (float xx = -1.0f; xx < 1.0f; xx += 2.0f / vertices) {

			float yeww = -(Math::pow(xx, 2.0f)) + 1.0f;
			positions.push_back({ xx, yeww});
			Debug{} << xx << ", " << yeww;
		}

		GL::Buffer vertexBuffer;
		vertexBuffer.setData(std::move(positions), GL::BufferUsage::StaticDraw);

		/* Configure the mesh, add vertex buffer */
		_mesh.setPrimitive(MeshPrimitive::LineStrip)
			.addVertexBuffer(std::move(vertexBuffer), 0, Shaders::Flat2D::Position{})
			.setCount(positions.size());

	}

};