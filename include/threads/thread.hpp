#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include "core/unique_ptr.hpp"

namespace mhe {

class thread;

class ThreadImpl
{
public:
	virtual ~ThreadImpl() {}
	virtual bool start() = 0;
	virtual bool stop() = 0;
	virtual bool join() = 0;
	virtual void set_thread(thread* thr) = 0;
};

class MHE_EXPORT thread
{
public:
	thread();
	virtual ~thread() {}

	bool start()
	{
		if (!start_impl())
			return false;
		finished_ = false;
		impl_->set_thread(this);
		return impl_->start();
	}

	bool stop()
	{
		finished_ = true;
		return impl_->stop();
	}

	bool join()
	{
		return impl_->join();
	}

	void process()
	{
		while (!finished_)
			process_impl();
	}

	bool finished() const
	{
		return finished_;
	}

	static size_t hardware_threads_number();
	static void sleep(size_t ms);
private:
	virtual bool start_impl() = 0;
	virtual void process_impl() = 0;

	unique_ptr<ThreadImpl> impl_;
	bool finished_;
};

class ConditionVariableImpl
{
public:
	virtual ~ConditionVariableImpl() {}
	virtual bool wait() = 0;
	virtual void notify() = 0;
	virtual bool check() const = 0;
};

class MHE_EXPORT condition_variable
{
public:
	condition_variable();

	bool wait()
	{
		return impl_->wait();
	}

	void notify()
	{
		impl_->notify();
	}

	bool check() const
	{
		return impl_->check();
	}
private:
	unique_ptr<ConditionVariableImpl> impl_;
};

}

#endif
