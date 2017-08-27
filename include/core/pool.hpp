#ifndef __POOL_HPP__
#define __POOL_HPP__

#include <iostream>
#include <cassert>
#include <algorithm>
#include "core/dynarray.hpp"
#include "core/assert.hpp"

typedef unsigned short uint16_t;

namespace mhe {

template <class T, class I = typename T::IdType>
struct StructTypePolicy
{
    static void set(T& obj, I id)
    {
        obj.id = id;
    }

    static I get(const T& obj)
    {
        return obj.id;
    }

    static void destroy(T&)
    {}
};

template <class T, class I>
struct ClassTypePolicy
{
    static void set(T& obj, I id)
    {
        obj.set_id(id);
    }

    static I get(const T& obj)
    {
        return obj.id();
    }

    static void destroy(T& obj)
    {
        obj.close();
    }
};

// IMPORTANT: Ideally this class should be used with structures. However it's possible to
// create a pool with T that equals to some class with a restriction that this classes must not contain virtual functions.
template < class T, size_t C = 0, class I = uint16_t, class Policy = ClassTypePolicy<T, I> >
class Pool
{
    struct Index
    {
        I id;       // index in indexes array
        I index;    // index of element in element's array
        I next;
    };
public:
    typedef T type;
    typedef I index_type;
    typedef Policy TypeTraits;

    static const I invalid_id = static_cast<I>(-1);

    Pool(allocator* alloc = default_allocator()) : objects_(nullptr), indexes_(nullptr), size_(0), capacity_(0), allocator_(alloc)
    {
        if (C != 0)
            resize(C);
    }

    ~Pool()
    {
        destroy_array(objects_, allocator_);
        destroy_array(indexes_, allocator_);
    }

    void resize(size_t size)
    {
        ASSERT(size_ == 0, "Trying to resize a pool that is already in use");
        capacity_ = static_cast<I>(size);
        destroy_array(objects_, allocator_);
        destroy_array(indexes_, allocator_);
        objects_ = create_array<T>(size, allocator_);
        indexes_ = create_array<Index>(size, allocator_);
        clear();
    }

    I create() const
    {
        ASSERT(size_ < capacity_, "Can't get next element from the pool");
        Index& ind = indexes_[first_];
        ind.index = size_++;
        T& obj = objects_[ind.index];
        Policy::set(obj, ind.id);
        first_ = ind.next;

        return ind.id;
    }

    T& get(I id) const
    {
        Index& ind = indexes_[id];
        ASSERT(ind.index != invalid_id && Policy::get(objects_[ind.index]) == id, "Invalid id " << id);
        return objects_[ind.index];
    }

    void remove(I id)
    {
        Index& ind = indexes_[id];
        assert(ind.index != invalid_id);
        // swap with the last object
        T& obj = objects_[ind.index];
        T& last = objects_[--size_];
        T tmp = obj;
        obj = last;
        last = tmp;
        indexes_[Policy::get(obj)].index = ind.index;

        ind.index = invalid_id;
        indexes_[last_].next = id;
        last_ = id;

        // Bad idea, but it helps me to prevent destructor's possible side effects
        memset(&tmp, 0, sizeof(T));
    }

    void make_invalid(I& id)
    {
        id = invalid_id;
    }

    bool is_valid(I id) const
    {
        return id != invalid_id && indexes_[id].index != invalid_id;
    }

    T* all_objects() const
    {
        return &objects_[0];
    }

    template <class Y>
    size_t all_indexes(Y* indexes, size_t count) const
    {
        size_t result = min(count, static_cast<Y>(size_));
        for (size_t i = 0; i < result; ++i)
            indexes[i] = indexes_[i].index;
        return result;
    }

    I size() const
    {
        return size_;
    }

    void update()
    {
        for (I i = 0; i < size_; ++i)
            indexes_[Policy::get(objects_[i])].index = i;
    }

    void clear()
    {
        first_ = 0;
        last_ = capacity_ - 1;
        size_ = 0;

        for (I i = 0; i < capacity_; ++i)
        {
            indexes_[i].id = i;
            indexes_[i].next = i + 1;
            indexes_[i].index = invalid_id;
        }
    }
private:
    mutable T* objects_;
    mutable Index* indexes_;
    mutable I first_;
    mutable I last_;
    mutable I size_;
    I capacity_;
    allocator* allocator_;
};

template <class T>
struct InvalidHandle
{
    static const T id;

    operator T()
    {
        return id;
    }
};

template <class T>
const T InvalidHandle<T>::id = static_cast<T>(-1);

template <class T>
bool is_handle_valid(T h)
{
    return h != InvalidHandle<T>::id;
}

template <class P>
class pool_initializer
{
public:
    typedef P Pool;
    typedef typename Pool::type Type;
    typedef typename Pool::index_type Index;
    typedef typename Pool::TypeTraits DestroyerTraits;

    explicit pool_initializer(Pool& pool) :
        pool_(pool)
    {
        index_ = pool.create();
    }

    explicit pool_initializer(const pool_initializer& other) :
        pool_(other.pool_),
        index_(other.index_)
    {
        other.take();
    }

    ~pool_initializer()
    {
        if (index_ != Pool::invalid_id)
        {
            DestroyerTraits::destroy(pool_.get(index_));
            pool_.remove(index_);
            index_ = Pool::invalid_id;
        }
    }

    Index take()
    {
        ASSERT(index_ != Pool::invalid_id, "Trying to take the index from the invalid pool_initializer");
        Index result = index_;
        index_ = Pool::invalid_id;
        return result;
    }

    Index get() const
    {
        ASSERT(index_ != Pool::invalid_id, "Trying to get the index from the invalid pool_initializer");
        return index_;
    }

    Type& object() const
    {
        ASSERT(index_ != Pool::invalid_id, "Trying to get the object from the invalid pool_initializer");
        return pool_.get(index_);
    }

    Pool& pool() const
    {
        return pool_;
    }
private:
    pool_initializer& operator= (const pool_initializer&);

    mutable Index index_;
    Pool& pool_;
};

template <class P>
pool_initializer<P> create_from_pool(P& pool)
{
    return pool_initializer<P>(pool);
}

template <class P>
void destroy_pool_object(P& pool, typename P::index_type& id)
{
    if (id == P::invalid_id) return;
    pool.get(id).close();
    pool.remove(id);
    pool.make_invalid(id);
}

}

#endif
