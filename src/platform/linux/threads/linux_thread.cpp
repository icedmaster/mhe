#include "threads/thread.hpp"

#include <pthread.h>

namespace mhe {

namespace {

void* start_thread_impl(void* param)
{
	thread* thr = reinterpret_cast<thread*>(param);
	thr->process();
	return 0;
}

}

struct thread::Info
{
	pthread_t thr;
	int id;
};

struct condition_variable::Info
{
	int id;
};

thread::thread() :
	info_(new Info)
{}

thread::~thread()
{}

bool thread::start_thread()
{
	info_->id = pthread_create(&info_->thr, 0, start_thread_impl, this);
	return info_->id == 0;
}

bool thread::stop()
{
	finished_ = true;
//return TerminateThread(info_->id, 0) != 0;
	return true;
}

bool thread::join()
{
	return pthread_join(info_->thr, 0);
}

void thread::process()
{
	while (!finished_)
		process_impl();
}

size_t thread::hardware_threads_number()
{
	return 1;
}

condition_variable::condition_variable() :
	info_(new Info)
{
}

bool condition_variable::wait()
{
    return false;
}

void condition_variable::notify()
{
}

}
