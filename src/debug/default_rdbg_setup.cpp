#include "debug/default_rdbg_setup.hpp"

#include "debug/rdbg.hpp"
#include "debug/light_wrapper.hpp"

namespace mhe {
	void default_rdbg_setup(RDBGProcessor& processor)
	{
		register_light_type(processor);
	}
}