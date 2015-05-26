#ifndef __GRID_HPP__
#define __GRID_HPP__

#include "math/vector3.hpp"
#include "allocator.hpp"
#include "assert.hpp"
#include "fixed_size_vector.hpp"

namespace mhe {

class allocator;

template <class E, size_t S>
struct Cell
{
	typedef E Element;
	fixed_size_vector<E, S> elements;

	Cell(allocator* alloc = default_allocator()) : elements(alloc) {}

	void add(const E& e)
	{
		//ASSERT(elements.size() < S, "Too many elements in the cell");
		elements.push_back(e);
	}

	size_t size() const
	{
		return elements.size();
	}

	template <class Functor>
	void iterate(Functor& f)
	{
		for (size_t i = 0; i < elements.size(); ++i)
			f(elements[i]);
	}
};

template <class G, bool B>
struct GridIteratorTraits
{};

template <class G>
struct GridIteratorTraits<G, false>
{
	typedef G Grid;
	typedef typename Grid::Size GridSize;
	typedef typename Grid::CellElement Cell;
};

template <class G>
struct GridIteratorTraits<G, true>
{
	typedef const G Grid;
	typedef typename Grid::Size GridSize;
	typedef const typename Grid::Cell Cell;
};

template <class C, class S = uint32_t>
class grid
{
public:
	typedef vector3<S> Size;
	typedef C Cell;
	typedef typename C::Element CellElement;
	typedef grid<C, S> this_type;

	template <class Traits>
	class Iterator
	{
	public:
		typedef typename Traits::CellElement CellElement;

		Iterator(typename Traits::Grid& grid, const typename Traits::GridSize& pos) : grid_(grid), pos_(pos) {}

		Cell* operator->()
		{
			return grid_.get(pos_);
		}
	private:
		typename Traits::Grid& grid_;
		typename Traits::GridSize pos_;
	};

	grid(allocator* alloc = default_allocator()) :
		cells_(nullptr), allocator_(alloc)
	{
	}

	grid(S dim, allocator* alloc = default_allocator()) :
		cells_(nullptr), allocator_(alloc)
	{
		resize(dim, dim, dim, C(allocator_));
	}

	grid(S dim, const C& value, allocator* alloc = default_allocator()) :
		cells_(nullptr), allocator_(alloc)
	{
		resize(dim, dim, dim, value);
	}

	const vector3<S>& size() const
	{
		return size_;
	}

	C& get(S x, S y, S z)
	{
		ASSERT(is_inside(x, y, z), "Invalid cell position");
		return cells_[x * size_.y() * size_.z() + y * size_.z() + z];
	}

	bool is_inside(S x, S y, S z) const
	{
		return x < size_.x() && y < size_.y() && z < size_.z();
	}

	bool is_inside(const Size& s) const
	{
		return is_inside(s.x(), s.y(), s.z());
	}

	void resize(S x, S y, S z, const C& initial_value)
	{
		allocator_->free(cells_);
		size_.set(x, y, z);
		S total = x * y * z;
		cells_ = new (allocator_) C[total];
		for (S i = 0; i < total; ++i)
			cells_[i] = initial_value;
	}

	void add(const CellElement& e, S x, S y, S z)
	{
		ASSERT(is_inside(x, y, z), "Invalid cell position");
		get(x, y, z).add(e);
	}

	void add(const CellElement& e, const Size& lowest, const Size& highest)
	{
		ASSERT(is_inside(lowest) && is_inside(highest), "Invalid bounds");
		for (S x = lowest.x(); x <= highest.x(); ++x)
		{
			for (S y = lowest.y(); y <= highest.y(); ++y)
			{
				for (S z = lowest.z(); z <= highest.z(); ++z)
					get(x, y, z).add(e);
			}
		}
	}

	template <class Functor>
	void iterate(const Size& lowest, const Size& highest, Functor& f)
	{
		ASSERT(is_inside(lowest) && is_inside(highest), "Invalid bounds");
		for (S x = lowest.x(); x <= highest.x(); ++x)
		{
			for (S y = lowest.y(); y <= highest.y(); ++y)
			{
				for (S z = lowest.z(); z <= highest.z(); ++z)
				{
					get(x, y, z).iterate(f);
				}
			}
		}
	}
private:
	C* cells_;
	vector3<S> size_;
	allocator* allocator_;
};

}

#endif
