#ifndef __APPLICATION2D_HPP__
#define __APPLICATION2D_HPP__

#include "application.hpp"

namespace mhe {

class Application2D : public Application
{
public:
	virtual ~Application2D() {}
protected:
	virtual bool mhe_app_init(const Config& config);
};

}

#endif
