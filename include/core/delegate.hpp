#ifndef __DELEGATE_HPP__
#define __DELEGATE_HPP__

#include <list>
#include <boost/shared_ptr.hpp>

namespace mhe {

// 0
#define NUMBER_OF_ARGS		0
#define TEMPLATE_ARGS		
#define TEMPLATE_INST_ARGS	
#define METHOD_ARGS			
#define ARGS				

#include "delegate_p.hpp"

#undef NUMBER_OF_ARGS
#undef TEMPLATE_ARGS
#undef TEMPLATE_INST_ARGS
#undef METHOD_ARGS
#undef ARGS

// 1
#define NUMBER_OF_ARGS		1
#define TEMPLATE_ARGS		, class T1
#define TEMPLATE_INST_ARGS	, T1
#define METHOD_ARGS			T1 arg1
#define ARGS				arg1

#include "delegate_p.hpp"

#undef NUMBER_OF_ARGS
#undef TEMPLATE_ARGS
#undef TEMPLATE_INST_ARGS
#undef METHOD_ARGS
#undef ARGS

// 2
#define NUMBER_OF_ARGS		2
#define TEMPLATE_ARGS		, class T1, class T2
#define TEMPLATE_INST_ARGS	, T1, T2
#define METHOD_ARGS			T1 arg1, T2 arg2
#define ARGS				arg1, arg2

#include "delegate_p.hpp"

#undef NUMBER_OF_ARGS
#undef TEMPLATE_ARGS
#undef TEMPLATE_INST_ARGS
#undef METHOD_ARGS
#undef ARGS

}

#endif
