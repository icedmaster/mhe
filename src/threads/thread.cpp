#include "threads/thread.hpp"

#include "core/config.hpp"
#include "threads/thread_impl.hpp"

namespace mhe {

thread::thread() :
	impl_(details::create_thread_impl())
{}

size_t thread::hardware_threads_number()
{
	return details::hardware_threads_number();
}

void thread::sleep(size_t ms)
{
	details::sleep(ms);
}

condition_variable::condition_variable() :
	impl_(details::create_condition_variable_impl())
{}

}
