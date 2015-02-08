#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <vector>
#include "mesh.hpp"

namespace mhe {

class RenderCommand
{
public:
	virtual ~RenderCommand() {}
	virtual bool execute() = 0;
};

struct Node
{
};

}

#endif
