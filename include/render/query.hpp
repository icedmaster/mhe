#ifndef __QUERY_HPP__
#define __QUERY_HPP__

#include "core/unique_ptr.hpp"

namespace mhe {

enum
{
	query_timestamp,
	query_time_elapsed,
	query_max
};

class QueryImpl
{
public:
	virtual ~QueryImpl() {}
	virtual bool init(int target) = 0;
	virtual void destroy() = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void get(int& res) const = 0;
	virtual void get(uint64_t& res) const = 0;
	virtual void set() = 0;
};

class Query
{
public:
	Query();

	bool init(int target)
	{
		return impl_->init(target);
	}

	void destroy()
	{
		impl_->destroy();
	}

	void begin()
	{
		impl_->begin();
	}

	void end()
	{
		impl_->end();
	}

	template <class T>
	T get() const
	{
		T t = T();
		impl_->get(t);
		return t;
	}

	void set()
	{
		impl_->set();
	}
private:
	unique_ptr<QueryImpl> impl_;
};

}

#endif
