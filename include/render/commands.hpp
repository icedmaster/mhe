#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "node.hpp"

namespace mhe {

class Driver;

class MHE_EXPORT ClearCommand : public RenderCommand
{
public:
	void set_driver(Driver* driver)
	{
		driver_ = driver;
	}

    bool execute();
    void reset();
private:
	Driver* driver_;
    bool executed_;
};

class MHE_EXPORT ClearCommandSimple : public RenderCommand
{
public:
	void set_driver(Driver* driver)
	{
		driver_ = driver;
	}

    bool execute();
private:
	Driver* driver_;
};

}

#endif