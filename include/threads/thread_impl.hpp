#ifndef __THREAD_IMPL_HPP__
#define __THREAD_IMPL_HPP__

#include "thread.hpp"

namespace mhe {

namespace details {

ThreadImpl* create_thread_impl();

size_t hardware_threads_number();
void sleep(size_t ms);

ConditionVariableImpl* create_condition_variable_impl();
MutexImpl* create_mutex_impl();

}}

#endif
