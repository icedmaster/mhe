#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include "core/unique_ptr.hpp"

namespace mhe {

class thread
{
	struct Info;
public:
	thread();
    virtual ~thread();

	bool start()
	{
		if (!start_impl())
			return false;
		finished_ = false;
		return start_thread();
	}
	bool stop();

	bool join();

	void process();

	static size_t hardware_threads_number();
private:
	virtual bool start_impl() = 0;
	virtual void process_impl() = 0;

	bool start_thread();
	
	unique_ptr<Info> info_;
	bool finished_;
};

class condition_variable
{
	struct Info;
public:
	condition_variable();

	bool wait();
	void notify();
private:
	unique_ptr<Info> info_;
};

}

#endif
