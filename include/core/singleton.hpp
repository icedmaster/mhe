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
        ASSERT(self_ == nullptr, "Singleton has already been created");
        self_ = new T;
    }

    static void destroy_singleton()
    {
        delete self_;
        self_ = nullptr;
    }

    static T& instance()
    {
        ASSERT(self_ != nullptr, "Invalid singleton");
        return *self_;
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
