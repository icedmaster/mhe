#ifndef __ALLOCATOR_HPP__
#define __ALLOCATOR_HPP__

#include "memory.hpp"
#include "assert.hpp"

#ifdef MHE_ALLOCATORS_DEBUG
#include "string.hpp"
#endif

namespace mhe {

class allocator
{
public:
	virtual ~allocator() {}

	virtual void* alloc(size_t size) = 0;
	virtual void free(void* ptr) = 0;
	virtual size_t allocated() const = 0;

	template <class T>
	T* create()
	{
		T* p = alloc(sizeof(T));
		return new (p) T;
	}

	template <class T>
	void destroy(T* ptr)
	{
		if (ptr == nullptr) return;
		ptr->~T();
		free(ptr);
	}

	template <class T, size_t N>
	T* create_array()
	{
		return create_array<T>(N);
	}

	template <class T>
	T* create_array(size_t n)
	{
		size_t* sptr = reinterpret_cast<size_t*>(alloc(sizeof(T) * n + sizeof(size_t)));
		*sptr = n;
		++sptr;
		T* ptr = reinterpret_cast<T*>(sptr);
		T* p = ptr;
		for (size_t i = 0; i < n; ++i, ++p)
			new (p) T;
		return ptr;
	}

	template <class T>
	void destroy_array(T* ptr)
	{
		if (ptr == nullptr) return;
		size_t* sptr = reinterpret_cast<size_t*>(ptr);
		--sptr;
		size_t n = *sptr;
		for (size_t i = 0; i < n; ++i, ++ptr)
			ptr->~T();
		free(sptr);
	}
#ifdef MHE_ALLOCATORS_DEBUG
	allocator(const string& name) : name_(name) {}

	const string& name() const
	{
		return name_;
	}
private:
	string name_;
#endif
};

class sysallocator : public allocator
{
public:
#ifdef MHE_ALLOCATORS_DEBUG
	sysallocator(const string& name) : allocator(name) {}
#endif

	void* alloc(size_t size) override
	{
		return mhe::allocate(size);
	}

	void free(void* ptr) override
	{
		return mhe::free(ptr);
	}

	size_t allocated() const override
	{
		return (size_t)mhe::allocated_memory_size();
	}
};

template <bool Align = true>
class fixed_size_allocator_base : public allocator
{
public:
#ifndef MHE_ALLOCATORS_DEBUG
	fixed_size_allocator_base() : ptr_(0), allocated_(0) {}
#else
	fixed_size_allocator_base(const string& name) : allocator(name), ptr_(0), allocated_(0) {}
#endif

	void* alloc(size_t size) override
	{
		if (Align) align_ptr();
		size_t res_ptr = ptr_;
		size_t new_ptr = ptr_ + size;
		if (new_ptr >= size_)
		{
			ASSERT(0, "Can't allocate " << size << " memory using fixed size allocator");
			return 0;
		}
		allocated_ += size;
		ptr_ = new_ptr;
		return (void*)(stack_ + res_ptr);
	}

	void free(void* /*ptr*/) override
	{
	}

	size_t allocated() const override
	{
		return allocated_;
	}

	void reset()
	{
		ptr_ = 0;
		allocated_ = 0;
	}
protected:
	void set(uint8_t* stack, size_t size)
	{
		stack_ = stack;
		size_ = size;
	}
private:
	void align_ptr()
	{
		size_t aligned = ptr_ & 0xfffffff0;
		if (aligned != ptr_)
			ptr_ = aligned + 0x10;
	}

	uint8_t* stack_;
	size_t size_;
	size_t ptr_;
	size_t allocated_;
};

template <size_t S, bool Align = true>
class stack_allocator : public fixed_size_allocator_base<Align>
{
public:
#ifndef MHE_ALLOCATORS_DEBUG
	stack_allocator()
#else
	stack_allocator(const string& name) : fixed_size_allocator_base<Align>(name)
#endif
	{
        set(stack_, S);
	}
private:
	uint8_t stack_[S];
};

template<bool Align = true>
class fixed_size_allocator : public fixed_size_allocator_base<Align>
{
public:
#ifndef MHE_ALLOCATORS_DEBUG
	fixed_size_allocator(size_t size) :
#else
	fixed_size_allocator(const string& name, size_t size) : fixed_size_allocator_base<Align>(name),
#endif
		stack_(new uint8_t[size])
	{
        fixed_size_allocator_base<Align>::set(stack_, size);
	}

	~fixed_size_allocator()
	{
		delete [] stack_;
	}
private:
	uint8_t* stack_;
};

#ifndef MHE_HEADERS_ONLY
void set_default_allocator(allocator* alloc);
MHE_EXPORT allocator* default_allocator();
MHE_EXPORT void create_default_allocator();
MHE_EXPORT void destroy_default_allocator();
#else
inline allocator* default_allocator()
{
	return nullptr;
}
#endif

template <class T>
inline T* create(allocator* alloc)
{
	return alloc->create<T>();
}

template <class T>
inline void destroy(T* ptr, allocator* alloc)
{
	return alloc->destroy<T>(ptr);
}

template <class T>
inline T* create_array(size_t n, allocator* alloc)
{
	return alloc->create_array<T>(n);
}

template <class T, size_t N>
inline T* create_array(allocator* alloc)
{
	return alloc->create_array<T, N>();
}

template <class T>
inline void destroy_array(T* ptr, allocator* alloc)
{
	alloc->destroy_array<T>(ptr);
}

}

inline void* operator new(size_t /*size*/, mhe::allocator* /*alloc*/) throw()
{
	ASSERT(0, "Do not use new[] with custom allocator");
	return nullptr;
}

inline void* operator new[](size_t size, mhe::allocator* alloc)
{
	void* p = alloc->alloc(size);
	return p;
}

inline void operator delete(void* ptr, mhe::allocator* alloc)
{
	alloc->free(ptr);
}

inline void operator delete[](void* /*ptr*/, mhe::allocator* /*alloc*/)
{
	ASSERT(0, "Do not use delete[] with custom allocator");
}

#endif
