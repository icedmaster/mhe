#ifndef __REF_PTR_HPP__
#define __REF_PTR_HPP__

#include "ref_counter.hpp"
#include "assert.hpp"

namespace mhe {

template <class T>
class ref_ptr
{
public:
    ref_ptr() :
        ptr_(nullptr)
    {}

    template <class Y>
    explicit ref_ptr(Y* p) :
        ptr_(p)
    {
        if (ptr_ != nullptr)
            ptr_->add_ref();
    }

    template <class Y>
    ref_ptr(const ref_ptr<Y>& p) :
        ptr_(p.get())
    {
        if (ptr_ != nullptr)
            ptr_->add_ref();
    }

    ref_ptr(const ref_ptr& p) :
        ptr_(p.get())
    {
        if (ptr_ != nullptr)
            ptr_->add_ref();
    }

    ~ref_ptr()
    {
        if (ptr_ != nullptr && !ptr_->rem_ref())
            delete ptr_;
        ptr_ = nullptr;
    }

    template <class Y>
    ref_ptr& operator= (Y* ptr)
    {
        set(ptr);
        return *this;
    }

    template <class Y>
    ref_ptr& operator= (ref_ptr<Y>& ptr)
    {
        set(ptr.ptr_);
        return *this;
    }

    ref_ptr& operator= (const ref_ptr& ptr)
    {
        set(ptr.ptr_);
        return *this;
    }

    template <class Y>
    bool operator== (const Y* ptr) const
    {
        return ptr_ == ptr;
    }

    template <class Y>
    bool operator== (const ref_ptr<Y>& ptr) const
    {
        return ptr_ == ptr.ptr_;
    }

    template <class Y>
    bool operator!= (Y* ptr) const
    {
        return ptr_ != ptr;
    }

    template <class Y>
    bool operator!= (const ref_ptr<Y>& ptr) const
    {
        return ptr_ != ptr.ptr_;
    }

#ifdef MHE_CPP11
    bool operator!= (std::nullptr_t p)
    {
        return ptr_ != p;
    }
#endif

    operator bool() const
    {
        return ptr_ != nullptr;
    }

    T* get() const
    {
        return ptr_;
    }

    T* operator->()
    {
        ASSERT(ptr_ != nullptr, "Invalid pointer");
        return ptr_;
    }

    const T* operator->() const
    {
        ASSERT(ptr_ != nullptr, "Invalid pointer");
        return ptr_;
    }

    T& operator* ()
    {
        ASSERT(ptr_, "Invalid pointer");
        return *ptr_;
    }

    const T& operator* () const
    {
        ASSERT(ptr_, "Invalid pointer");
        return *ptr_;
    }
private:
    template <class Y>
    void set(Y* ptr)
    {
        if (ptr == ptr_) return;
        if (ptr_ != nullptr && !ptr_->rem_ref())
            delete ptr_;
        ptr_ = ptr;
        if (ptr != nullptr) ptr->add_ref();
    }

    T* ptr_;
};

}

#endif 
