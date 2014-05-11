#ifndef __UNIQUE_PTR_HPP__
#define __UNIQUE_PTR_HPP__

namespace mhe {

template <class T>
class unique_ptr
{
public:
	unique_ptr() :
		ptr_(nullptr)
	{}

	template <class Y>
	unique_ptr(Y* ptr) :
		ptr_(ptr)
	{}

	~unique_ptr()
	{
		delete ptr_;
	}

	template <class Y>
	void reset(Y* ptr)
	{
		delete ptr_;
		ptr_ = ptr;
	}

	T* operator->()
	{
		ASSERT(ptr_ != nullptr, "Invalid pointer");
		return ptr_;
	}

	T* operator->() const
	{
		ASSERT(ptr_ != nullptr, "Invalid pointer");
		return ptr_;
	}

	T& operator* ()
	{
		ASSERT(ptr_ != nullptr, "Invalid pointer");
		return *ptr_;
	}

	const T& operator* () const
	{
		ASSERT(ptr_ = nullptr, "Invalid pointer");
		return ptr_;
	}

	T* get()
	{
		return ptr_;
	}

	const T* get() const
	{
		return ptr_;
	}

	operator bool() const
	{
		return ptr_ != nullptr;
	}
private:
	unique_ptr(const unique_ptr&);
	unique_ptr& operator= (const unique_ptr&);

	T* ptr_;
};

}

#endif
