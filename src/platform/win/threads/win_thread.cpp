#include "threads/thread.hpp"

#include "platform/win/win_wrapper.hpp"

namespace mhe {

namespace {

DWORD WINAPI start_thread(void* param)
{
	thread* thr = reinterpret_cast<thread*>(param);
	thr->process();
	return 0;
}

}

struct thread::Info
{
	HANDLE id;
};

struct condition_variable::Info
{
	HANDLE id;
};

thread::thread() :
	info_(new Info)
{}

bool thread::start()
{
	finished_ = false;
	info_->id = CreateThread(NULL, 0, start_thread, this, 0, 0);
	return info_->id != FALSE;
}

bool thread::stop()
{
	finished_ = true;
	return TerminateThread(info_->id, 0) != 0;
}

bool thread::join()
{
	return WaitForSingleObject(info_->id, INFINITE) == WAIT_OBJECT_0;
}

void thread::process()
{
	while (!finished_)
		process_impl();
}

size_t thread::hardware_threads_number()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo( &sysInfo );

	return sysInfo.dwNumberOfProcessors;
}

condition_variable::condition_variable() :
	info_(new Info)
{
	info_->id = CreateEvent(NULL, FALSE, FALSE, NULL);
}

bool condition_variable::wait()
{
	return WaitForSingleObject(info_->id, INFINITE) != FALSE;
}

void condition_variable::notify()
{
	SetEvent(info_->id);
}

}
