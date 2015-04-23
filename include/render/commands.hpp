#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "node.hpp"

namespace mhe {

class Driver;
class RenderTarget;

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

class MHE_EXPORT SetRenderTargetTextureSideCommand : public RenderCommand
{
public:
	void setup(Driver* driver, RenderTarget* render_target, int side)
	{
		driver_ = driver;
		render_target_ = render_target;
		side_ = side;
		executed_ = false;
	}

	bool execute();
private:
	Driver* driver_;
	RenderTarget* render_target_;
	int side_;
	bool executed_;
};

}

#endif