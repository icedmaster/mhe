#ifndef __MESH_GRID_HPP__
#define __MESH_GRID_HPP__

#include "core/grid.hpp"
#include "utils/pool_utils.hpp"
#include "math/ray.hpp"
#include "math/math_utils.hpp"
#include "render_common.hpp"

namespace mhe {

inline vector3<int> ceil(const vec3& v)
{
	return vector3<int>(::ceil(v.x()), ::ceil(v.y()), ::ceil(v.z()));
}

struct MeshGridTriangle
{
	vec3 pos[3];
	vec3 nrm[3];
};

template <class Tri>
class MeshGridBase : public grid<Tri, uint16_t>
{
    typedef grid<Tri, uint16_t> BaseGrid;
public:
	MeshGridBase(allocator* alloc) :
		grid<Tri, uint16_t>(alloc)
		{}

	void init(const Tri* triangles, size_t triangles_number, float cell_size)
	{
		vec3 min_value(999999.0f), max_value(-999999.0f);
		for (size_t i = 0, n = triangles_number; i < n; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				min_value = mhe::min(min_value, triangles[i].pos[j]);
				max_value = mhe::max(max_value, triangles[i].pos[j]);
			}
		}

		vector3<int> min_cell = ceil(min_value / cell_size);
		vector3<int> max_cell = ceil(max_value / cell_size);
		vector3<int> dimension = max_cell - min_cell + vector3<int>(1);
        BaseGrid::resize(dimension);

		min_cell_ = min_cell;
		max_cell_ = max_cell;
		cell_size_ = cell_size;

		fill(triangles, triangles_number);
	}

	template <class Iterator>
	void iterate(Iterator& it, const vec3& from, const vec3& to)
	{
		vector3<int> from_cell = calculate_cell(from);
		vector3<int> to_cell = calculate_cell(to);
		grid<trianglef, uint16_t>::iterate(it, mhe::min(from_cell, to_cell), mhe::max(from_cell, to_cell));
	}
private:
	vector3<int> calculate_cell(const vec3& pos) const
	{
		return clamp(ceil(pos / cell_size_), min_cell_, max_cell_) - min_cell_;
	}

	void fill(const Tri* triangles, size_t triangles_number)
	{
		for (size_t i = 0, n = triangles_number; i < n; ++i)
		{
			vec3 min_value(999999.0f), max_value(-999999.0f);
			for (int j = 0; j < 3; ++j)
			{
				min_value = mhe::min(min_value, triangles[i].pos[j]);
				max_value = mhe::max(max_value, triangles[i].pos[j]);
			}
            BaseGrid::add(triangles[i], calculate_cell(min_value), calculate_cell(max_value));
		}
	}

	vector3<int> min_cell_;
	vector3<int> max_cell_;
	float cell_size_;
};

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

typedef MeshGridBase<MeshGridTriangle> MeshGrid;

template <class Vertices, class Indices>
void create_grid(MeshGrid& grid, const Vertices& vertices, size_t /*vertices_number*/, const Indices& indices, size_t indices_number,
	allocator* alloc = default_allocator())
{
	ASSERT(indices_number % 3 == 0, "Only triangulated meshes are currently supported");
	MeshGridTriangle* triangles = alloc->create_array<MeshGridTriangle>(indices_number / 3);
	for (size_t i = 0, t = 0; i < indices_number; i +=3, ++t)
	{
		triangles[t].pos[0] = vertices[indices[i + 0]].pos;
		triangles[t].pos[1] = vertices[indices[i + 1]].pos;
		triangles[t].pos[2] = vertices[indices[i + 2]].pos;
		triangles[t].nrm[0] = vertices[indices[i + 0]].nrm;
		triangles[t].nrm[1] = vertices[indices[i + 1]].nrm;
		triangles[t].nrm[2] = vertices[indices[i + 2]].nrm;
	}

	grid.init(triangles, indices_number / 3, float_max);
	alloc->destroy_array(triangles);
}

struct MeshTraceDataInstance
{
	POOL_STRUCT(MeshTraceDataHandleType);
	MeshGrid grid;

	MeshTraceDataInstance() : id(invalid_id), grid(default_allocator()) {}
};

}

#endif
