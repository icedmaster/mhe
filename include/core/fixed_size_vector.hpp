#ifndef __FIXED_SIZE_VECTOR_HPP__
#define __FIXED_SIZE_VECTOR_HPP__

#include "compiler.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"

#ifdef MHE_VS
// TODO: need to check out this warning later
#pragma warning( disable: 4996 )    // copy may be unsafe
#endif

#include <iostream>
#include <cstring>
#include <sstream>
#include <cassert>

namespace mhe {

namespace detail {

struct default_reallocation_policy
{
    static void reallocate(size_t /*prev_size*/, size_t /*new_size*/, const char* /*additional_info*/) {}
};

struct log_reallocation_policy
{
    static void reallocate(size_t prev_size, size_t new_size, const char* additional_info)
    {
        std::cout << "reallocate(), " << prev_size << " " << new_size << " info:" << additional_info << std::endl;
    }
};

struct assert_reallocation_policy
{
    static void reallocate(size_t /*prev_size*/, size_t /*new_size*/, const char* /*additional_info*/)
    {
        assert(false);
    }
};

template <class T, size_t count>
struct static_array_traits
{
    T elements[count];
};

template <class T, size_t count>
struct dynamic_array_traits
{
    dynamic_array_traits() : elements(new T[count]) {}
    ~dynamic_array_traits() { delete [] elements; }
    T* elements;
};

}   // detail

#define DEFAULT_REALLOCATION_POLICY         detail::default_reallocation_policy

template <class T, size_t count, class reallocation_policy = DEFAULT_REALLOCATION_POLICY>
class fixed_size_vector
{
public:
    typedef fixed_size_vector<T, count> this_type;
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;
public:
    fixed_size_vector(allocator* alloc = default_allocator()) :
        begin_(elements_), size_(0), capacity_(count), allocator_(alloc)
    {}

    fixed_size_vector(const this_type& v, allocator* alloc = default_allocator()) :
        begin_(elements_), size_(0), capacity_(count), allocator_(alloc)
    {
        insert(end(), v.begin(), v.end());
    }

    fixed_size_vector(size_t size, allocator* alloc = default_allocator()) :
        begin_(elements_), size_(0), capacity_(count), allocator_(alloc)
    {       
        resize(size);
    }

    template <class InputIterator>
    fixed_size_vector(InputIterator first, InputIterator last, allocator* alloc = default_allocator()) :
        begin_(elements_), capacity_(count)
    {
        insert(end(), first, last);
    }

    ~fixed_size_vector()
    {
        if (begin_ != elements_)
        {
            free(begin_);
        }
    }

    void set_allocator(allocator* alloc)
    {
        if (elements_ != begin_)
        {
            ASSERT(0, "Can't set allocator - some data has been allocated already using different allocator");
            return;
        }
        allocator_ = alloc;
    }

    // accessors
    iterator begin()
    {
        return begin_;
    }

    const_iterator begin() const
    {
        return begin_;
    }

    iterator end()
    {
        return begin_ + size_;
    }

    const_iterator end() const
    {
        return begin_ + size_;
    }

    T& operator[] (size_t index)
    {
        return begin_[index];
    }

    const T& operator[] (size_t index) const
    {
        return begin_[index];
    }

    T& at(size_t index)
    {
        assert(index < size_);
        return begin_[index];
    }

    T at(size_t index) const
    {
        assert(index < size_);
        return begin_[index];
    }

    T& front()
    {
        return begin_[0];
    }

    const T& front() const
    {
        return begin_[0];
    }

    T& back()
    {
        return begin_[size_ - 1];
    }

    const T& back() const
    {
        return begin_[size_ - 1];
    }

    const T* data() const
    {
        return begin_;
    }

    size_t size() const
    {
        return size_;
    }

    size_t capacity() const
    {
        return capacity_;
    }

    bool empty() const
    {
        return !size_;
    }

    // inserting elements
    template <class InputIterator>
    iterator insert(const_iterator position, InputIterator first, InputIterator last)
    {
        size_t first_index = position - begin();
        size_t inserted_count = 0;
        for (; first != last; ++first)
            insert_impl(first_index + inserted_count++, *first);
        return begin_ + inserted_count;
    }

    iterator insert(const_iterator position, const T& value)
    {
        return insert_impl(position - begin(), value);
    }

    void push_back(const T& value)
    {
        insert(end(), value);
    }

    void append(const T* values, size_t size)
    {
        insert(end(), values, values + size);
    }

    void resize(size_t new_size)
    {
        if (new_size > capacity_)
            reallocate_vector(new_size);
        size_ = new_size;
    }

    void reserve(size_t capacity)
    {
        if (capacity > capacity_)
            reallocate_vector(capacity);
    }

    void fill(const T& value)
    {
        for (size_t i = 0; i < capacity_; ++i)
            begin_[i] = value;
    }

    // erasing

    void clear()
    {
        size_ = 0;
    }   

    iterator erase(const_iterator position)
    {
        return erase_impl(position - begin(), 1);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        size_t index = first - begin();
        size_t erased_count = 0;
        for (; first != last; ++first) ++erased_count;
        return erase_impl(index, erased_count);
    }

    // optimization, use this methods carefully
    iterator next_predefined_element()
    {
        size_t index = size_;       
        if (index >= capacity_)
            return end();
        ++size_;
        return begin_ + index;
    }

    T& add()
    {
        iterator it = next_predefined_element();
        ASSERT(it != end(), "Can not add a new element");
        return *it;
    }

    // operators
    this_type& operator= (const this_type& other)
    {
        if (&other == this)
            return *this;
        clear();
        insert(end(), other.begin(), other.end());
        return *this;
    }

    bool operator== (const this_type& other) const
    {
        if (size_ != other.size_) return false;
        for (size_t i = 0; i < size_; ++i)
        {
            if (begin_[i] != other.begin_[i]) return false;
        }
        return true;
    }

    bool operator!= (const this_type& other) const
    {
        return !(*this == other);
    }
private:
    iterator insert_impl(size_t index, const T& value)
    {
        if ((size_ + 1) > capacity_)
            reallocate_vector(capacity_ * 2);
        if (index != size_)
            mhe::copy(begin_ + index, begin_ + size_ - index, begin_ + index + 1);
        begin_[index] = value;
        ++size_;
        return begin_ + index;
    }

    void reallocate_vector(size_t new_capacity)
    {
        reallocation_policy::reallocate(capacity_, new_capacity, FUNCTION_DESCRIPTION_MACRO);
        T* copy = begin_;;
        capacity_ = new_capacity;
        begin_ = allocate(capacity_);
        mhe::copy(copy, copy + size_, begin_);
        if (copy != elements_) free(copy);
    }

    iterator erase_impl(size_t index, size_t erased_count)
    {
        if (index != (size_ - erased_count))
            mhe::copy(begin_ + index + erased_count, begin_ + size_, begin_ + index);
        size_ -= erased_count;
        return begin_ + index;
    }

    T* allocate(size_t n)
    {
        return create_array<T>(n, allocator_);
    }

    void free(T* ptr)
    {
        destroy_array(ptr, allocator_);
    }

    T elements_[count];
    T* begin_;
    size_t size_;
    size_t capacity_;
    allocator* allocator_;
};

template <class T, size_t count, class Traits = detail::static_array_traits<T, count> >
class fixed_capacity_vector
{
public:
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef T type;
public:
    fixed_capacity_vector() : size_(0) {}

    T& add()
    {
        ASSERT(size_ < count - 1, "fixed_capacity_vector is full");
        return traits_.elements[size_++];
    }

    void push_back(const T& v)
    {
        ASSERT(size_ < count - 1, "fixed_capacity_vector is full");
        traits_.elements[size_++] = v;
    }

    const T* data() const
    {
        return traits_.elements;
    }

    T* data()
    {
        return traits_.elements;
    }

    size_t size() const
    {
        return size_;
    }

    void clear()
    {
        size_ = 0;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    T& operator[] (size_t index)
    {
        ASSERT(index < size_, "Invalid index");
        return traits_.elements[index];
    }

    const T& operator[] (size_t index) const
    {
        ASSERT(index < size_, "Invalid index");
        return traits_.elements[index];
    }
private:
    Traits traits_;
    size_t size_;
};

template <class T, size_t count>
inline std::ostream& operator<< (std::ostream& stream, const fixed_size_vector<T, count>& vector)
{
    std::stringstream ss;
    ss << "{";
    for (size_t i = 0; i < vector.size(); ++i)
        ss << vector[i] << ", ";
    ss << "}";
    stream << ss.str();
    return stream;
}

}

#endif
