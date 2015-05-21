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
	float dist;
	vec3 res;
	bool found;
	Iterator(const rayf& r) : dist(float_max), found(false), ray_(r) {}

	void operator() (const trianglef& t)
	{
		vec3 current_res;
		float current_dist;
		bool is_intersects = intersects(current_res, current_dist, ray_, t);
		if (is_intersects && current_dist < dist)
		{
			res = current_res;
			dist = current_dist;
		}
		found = is_intersects;
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

		grid_.add(t, lowest, highest);
	}

	bool closest_intersection(vec3& res, const rayf& r) const
	{
		const MeshGrid::Size& size = grid_.size();
		vec3 s(size.x(), size.y(), size.z());
		MeshGrid::Size s1 = div(r.origin, s);
		MeshGrid::Size s2 = div(r.direction, s);
		MeshGrid::Size lowest = max(min(s1, s2), MeshGrid::Size::zero());
		MeshGrid::Size highest = min(max(s1, s2), size - MeshGrid::Size(1, 1, 1));

		Iterator it(r);
		grid_.iterate(lowest, highest, it);
		res = it.res;
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
void create_grid(MeshGrid& grid, const Vertices& vertices, const Indices& indices)
{
	MeshGridHelper helper(grid);
	ASSERT(indices.size() % 3 == 0, "Only triangulated meshes are currently supported");
	for (size_t i = 0; i < indices.size(); i +=3)
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
