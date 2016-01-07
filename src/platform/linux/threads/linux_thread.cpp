#include "threads/thread.hpp"

#include <pthread.h>
#include <unistd.h>

#include "utils/global_log.hpp"

namespace mhe {

class LinuxThreadImpl : public ThreadImpl
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
    pthread_t pthr_;
    int id_;
    thread* thr_;
};

class LinuxConditionVariable : public ConditionVariableImpl
{
public:
    LinuxConditionVariable();
    ~LinuxConditionVariable();

    bool wait() override;
    void notify() override;
    bool check() const override
    {
        return flag_;
    }
private:
    pthread_cond_t cond_;
    pthread_mutexattr_t mutex_attr_;
    pthread_mutex_t mutex_;
    bool flag_;
};

class LinuxMutex : public MutexImpl
{
public:
    LinuxMutex();
    ~LinuxMutex();

    bool lock() override;
    void unlock() override;
private:
    pthread_mutexattr_t mutex_attr_;
    pthread_mutex_t mutex_;
};

namespace {

void* start_thread_impl(void* param)
{
    thread* thr = reinterpret_cast<thread*>(param);
    thr->process();
    return 0;
}

}

bool LinuxThreadImpl::start()
{
    id_ = pthread_create(&pthr_, 0, start_thread_impl, thr_);
    return id_ == 0;
}

bool LinuxThreadImpl::stop()
{
    return true;
}

bool LinuxThreadImpl::join()
{
    return pthread_join(pthr_, 0);
}

LinuxConditionVariable::LinuxConditionVariable() : flag_(false)
{
    pthread_cond_init(&cond_, nullptr);
    pthread_mutexattr_init(&mutex_attr_);
    pthread_mutex_init(&mutex_, &mutex_attr_);
}

LinuxConditionVariable::~LinuxConditionVariable()
{
    pthread_mutexattr_destroy(&mutex_attr_);
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&cond_);
}

bool LinuxConditionVariable::wait()
{
    pthread_mutex_lock(&mutex_);
    if (flag_)
    {
        flag_ = false;
        pthread_mutex_unlock(&mutex_);
        return true;
    }
    if (pthread_cond_wait(&cond_, &mutex_) != 0)
        return false;
    flag_ = false;
    pthread_mutex_unlock(&mutex_);
    return true;
}

void LinuxConditionVariable::notify()
{
    pthread_mutex_lock(&mutex_);
    flag_ = true;
    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&cond_);
}

LinuxMutex::LinuxMutex()
{
    pthread_mutexattr_init(&mutex_attr_);
    pthread_mutex_init(&mutex_, &mutex_attr_);
}

LinuxMutex::~LinuxMutex()
{
    pthread_mutexattr_destroy(&mutex_attr_);
    pthread_mutex_destroy(&mutex_);
}

bool LinuxMutex::lock()
{
    return pthread_mutex_lock(&mutex_) == 0;
}

void LinuxMutex::unlock()
{
    pthread_mutex_unlock(&mutex_);
}

namespace details {
ThreadImpl* create_thread_impl()
{
    return new LinuxThreadImpl;
}

size_t hardware_threads_number()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

void sleep(size_t ms)
{
    usleep(ms * 1000);
}

ConditionVariableImpl* create_condition_variable_impl()
{
    return new LinuxConditionVariable;
}

MutexImpl* create_mutex_impl()
{
    return new LinuxMutex;
}

} // namespace details

}
