#include "platform/win/win_thread.hpp"

#include "platform/win/win_wrapper.hpp"

namespace mhe {

class WinThreadImpl : public ThreadImpl
{
public:
	bool start() override;
	bool stop() override;
	bool join() override;
	void set_thread(thread* thr)
	{
		thr_ = thr;
	}
private:
	HANDLE id_;
	thread* thr_;
};

class WinConditionVariable : public ConditionVariableImpl
{
public:
	WinConditionVariable();
	bool wait() override;
	void notify() override;
private:
	HANDLE id_; 
};

namespace {

DWORD WINAPI start_thread_impl(void* param)
{
	thread* thr = reinterpret_cast<thread*>(param);
	thr->process();
	return 0;
}

}

bool WinThreadImpl::start()
{
	id_ = CreateThread(NULL, 0, start_thread_impl, thr_, 0, 0);
	return id_ != FALSE;
}

bool WinThreadImpl::stop()
{
	return TerminateThread(id_, 0) != 0;
}

bool WinThreadImpl::join()
{
	return WaitForSingleObject(id_, INFINITE) == WAIT_OBJECT_0;
}

namespace details {
ThreadImpl* create_thread_impl()
{
	return new WinThreadImpl;
}

size_t hardware_threads_number()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo( &sysInfo );

	return sysInfo.dwNumberOfProcessors;
}

void sleep(size_t ms)
{
	::Sleep(ms);
}

ConditionVariableImpl* create_condition_variable_impl()
{
	return new WinConditionVariable;
}

} // namespace details

WinConditionVariable::WinConditionVariable() :
	id_(CreateEvent(NULL, FALSE, FALSE, NULL))
{}

bool WinConditionVariable::wait()
{
	return WaitForSingleObject(id_, INFINITE) != FALSE;
}

void WinConditionVariable::notify()
{
	SetEvent(id_);
}

}
