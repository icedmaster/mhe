#ifndef __APPLICATION2D_HPP__
#define __APPLICATION2D_HPP__

#include "application.hpp"

namespace mhe {
namespace app {

class Application2D : public Application
{
public:
	Application2D(const std::string& name) :
		Application(name)
	{}

	virtual ~Application2D() {}
protected:
	virtual int run_impl();
	virtual void init_impl();
};

}}

#endif
