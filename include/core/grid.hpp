#ifndef __GRID_HPP__
#define __GRID_HPP__

#include "math/vector3.hpp"
#include "allocator.hpp"
#include "assert.hpp"
#include "vector.hpp"
#include "fixed_size_vector.hpp"

namespace mhe {

class allocator;

template <class E, class I>
class grid
{
    struct Cell
    {
        typedef vector<I> Data;
        Data data;
    };

    typedef E Element;
    typedef I Index;
public:
    grid(allocator* alloc) :
        data_(alloc), cells_(nullptr), alloc_(alloc)
        {}

    void add(const Element& e, size_t x, size_t y, size_t z)
    {
        I index = static_cast<I>(data_.size());
        data_.push_back(e);
        size_t cell_index = z * dim_.x() * dim_.y() + y * dim_.x() + x;
        ASSERT(cell_index < dim_.x() * dim_.y() * dim_.z(), "Invalid index:" << index);
        cells_[cell_index].data.push_back(cell_index);
    }

    void add(const Element& e, const vector3<int>& from, const vector3<int>& to)
    {
        I index = static_cast<I>(data_.size());
        data_.push_back(e);
        for (int x = from.x(); x <= to.x(); ++x)
        {
            for (int y = from.y(); y <= to.y(); ++y)
            {
                for (int z = from.z(); z <= to.z(); ++z)
                {
                    size_t cell_index = z * dim_.x() * dim_.y() + y * dim_.x() + x;
                    ASSERT(cell_index < dim_.x() * dim_.y() * dim_.z(), "Invalid index:" << index);
                    cells_[cell_index].data.push_back(index);
                }
            }
        }
    }

    void resize(size_t dim)
    {
        dim_.set(dim, dim, dim);
        resize_impl();
    }

    void resize(const vector3<int>& dim)
    {
        dim_ = dim;
        resize_impl();
    }

    template <class Iterator>
    void iterate(Iterator& it, const vector3<int>& from, const vector3<int>& to)
    {
        for (int x = from.x(); x <= to.x(); ++x)
        {
            for (int y = from.y(); y <= to.y(); ++y)
            {
                for (int z = from.z(); z <= to.z(); ++z)
                {
                    size_t cell_index = z * dim_.x() * dim_.y() + y * dim_.x() + x;
                    ASSERT(cell_index < dim_.x() * dim_.y() * dim_.z(), "Invalid index:" << cell_index);
                    for (size_t i = 0, size = cells_[cell_index].data.size(); i < size; ++i)
                        it(data_[cells_[cell_index].data[i]]);
                }
            }
        }
    }

    const vector<E>& data() const
    {
        return data_;
    }
private:
    void resize_impl()
    {
        alloc_->destroy_array(cells_);
        size_t size = dim_.x() * dim_.y() * dim_.z();
        cells_ = alloc_->create_array<Cell>(size);
        for (size_t i = 0; i < size; ++i)
            cells_[i].data.set_allocator(alloc_);
    }

    vector<E> data_;
    Cell* cells_;
    vector3<size_t> dim_;
    allocator* alloc_;
};

}

#endif
