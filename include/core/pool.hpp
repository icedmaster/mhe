#ifndef __POOL_HPP__
#define __POOL_HPP__

namespace mhe {

template <class T, class Handle, size_t C = 128>
class Pool
{
public:
    Pool() :
		last_(0)
	{
	}

	Handle create()
	{
		return last_++;
	}

	void close(Handle handle)
	{
        ASSERT(handle < C, "Invalid handle:" << handle);
	}

	T& get(Handle handle)
	{
        ASSERT(handle < C, "Invalid handle:" << handle);
        return pool_[handle];
	}
private:
    T pool_[C];
	Handle last_;
};

}

#endif
