#ifndef __CONFIGURABLE_TYPES_HPP__
#define __CONFIGURABLE_TYPES_HPP__

#include "fixed_size_vector.hpp"
#include "engine_config.hpp"

#include "node.hpp"

namespace mhe {

typedef fixed_size_vector<nodeptr, initial_number_of_nodes> nodelist;

}

#endif
