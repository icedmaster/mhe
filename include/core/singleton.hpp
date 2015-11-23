#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

#include "noncopyable.hpp"
#include "allocator.hpp"

namespace mhe {

template <class T>
class Singleton : public noncopyable
{
public:
	static void create_singleton()
	{
		self_ = new T;
	}

	static void destroy_singleton()
	{
		delete self_;
	}

	static T& instance()
	{
		return *self_;
		//static T self;
		//return self;
	}
protected:
	Singleton() {}
	~Singleton() {}
private:
	static T* self_;
};

template <class T>
T* Singleton<T>::self_ = nullptr;

}

#endif
