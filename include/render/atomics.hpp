#ifndef __ATOMICS_HPP__
#define __ATOMICS_HPP__

#include "core/unique_ptr.hpp"

namespace mhe {

class AtomicImpl
{
public:
    virtual ~AtomicImpl() {}
    virtual bool init(int type) = 0;
    virtual void destroy() = 0;
};

class Atomic
{
public:
    Atomic();

    bool init(int type)
    {
        return impl_->init(type);
    }

    void destroy()
    {
        impl_->destroy();
    }

    const AtomicImpl* impl() const
    {
        return impl_.get();
    }
private:
    unique_ptr<AtomicImpl> impl_;
};

}

#endif
