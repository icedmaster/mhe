#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

#include "noncopyable.hpp"

namespace mhe {

template <class T>
class Singleton : public noncopyable
{
public:
	static T& instance()
	{
		static T self;
		return self;
	}
protected:
  	Singleton() {}
  	~Singleton() {}
};

}

#endif
