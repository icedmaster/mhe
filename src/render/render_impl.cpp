#include "render/query.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

Query::Query() :
	impl_(SystemFactory::instance().create_query())
{}

}
