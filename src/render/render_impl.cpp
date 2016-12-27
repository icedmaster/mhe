#include "render/query.hpp"
#include "render/atomics.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

Query::Query() :
    impl_(SystemFactory::instance().create_query())
{}

Atomic::Atomic() :
    impl_(SystemFactory::instance().create_atomic())
{}

}
