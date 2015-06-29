#ifndef __MESH_GRID_HPP__
#define __MESH_GRID_HPP__

#include "core/grid.hpp"
#include "math/ray.hpp"
#include "math/math_utils.hpp"
#include "render_common.hpp"

namespace mhe {

typedef Cell<trianglef, 64> MeshCell;
typedef grid<MeshCell> MeshGrid;

struct Iterator
{
	hitf h;
	bool found;
	Iterator(const rayf& r) : found(false), ray_(r)
	{
		h.distance = float_max;
	}

	void operator() (const trianglef& t)
	{
		vec3 current_res;
		hitf current_hit;
		bool is_intersects = intersects(current_hit, ray_, t);
		if (is_intersects && current_hit.distance < h.distance)
		{
			h = current_hit;
			found = true;
		}
	}
private:
	rayf ray_;
};

class MeshGridHelper
{
public:
	MeshGridHelper(MeshGrid& grid) : grid_(grid) {}

	void add(const vec3& p1, const vec3& p2, const vec3& p3)
	{
		const MeshGrid::Size& size = grid_.size();
		vec3 s(size.x(), size.y(), size.z());
		MeshGrid::Size s1 = div(p1, s);
		MeshGrid::Size s2 = div(p2, s);
		MeshGrid::Size s3 = div(p3, s);
		MeshGrid::Size lowest = min(min( s1, s2 ), s3);
		MeshGrid::Size highest = max(max( s1, s2 ), s3);
		trianglef t;
		t.vertices[0] = p1;
		t.vertices[1] = p2;
		t.vertices[2] = p3;

		grid_.add(t, MeshGrid::Size::zero(), MeshGrid::Size::zero());
	}

	bool closest_intersection(hitf& h, const rayf& r) const
	{
		const MeshGrid::Size& size = grid_.size();
		vec3 s(size.x(), size.y(), size.z());
		MeshGrid::Size s1 = div(r.origin, s);
		MeshGrid::Size s2 = div(r.direction * r.length, s);
		// TODO:
		MeshGrid::Size lowest = MeshGrid::Size::zero();
		MeshGrid::Size highest = MeshGrid::Size::zero();

		Iterator it(r);
		grid_.iterate(lowest, highest, it);
		h = it.h;
		return it.found;
	}
private:
	MeshGrid::Size div(const vec3& fv, const vec3& v) const
	{
		return MeshGrid::Size((fv.x() + v.x() * 0.5f), (fv.y() + v.y() * 0.5f), (fv.z() + v.z() * 0.5f));
	}

	MeshGrid& grid_;
};

template <class Vertices, class Indices>
void create_grid(MeshGrid& grid, const Vertices& vertices, size_t /*vertices_number*/, const Indices& indices, size_t indices_number)
{
	MeshGridHelper helper(grid);
	ASSERT(indices_number % 3 == 0, "Only triangulated meshes are currently supported");
	for (size_t i = 0; i < indices_number; i +=3)
	{
		helper.add(vertices[indices[i + 0]].pos,
			vertices[indices[i + 1]].pos, vertices[indices[i + 2]].pos);
	}
}

struct MeshTraceDataInstance
{
	POOL_STRUCT(MeshTraceDataHandleType);
	MeshGrid grid;

	MeshTraceDataInstance() : id(invalid_id) {}
};

}

#endif
