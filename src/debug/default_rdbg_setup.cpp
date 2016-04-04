#include "debug/default_rdbg_setup.hpp"

#include "debug/rdbg.hpp"
#include "debug/light_wrapper.hpp"
#include "debug/common_types_wrapper.hpp"

namespace mhe {

void default_rdbg_setup(RDBGProcessor& processor)
{
    register_common_types(processor);
    register_light_type(processor);
}
}