#ifndef __LABEL_HPP__
#define __LABEL_HPP__

#include "widget.hpp"

namespace mhe {
namespace gui {

class Label : public Widget
{
public:
	Label(const std::string& name) :
		Widget(name)
	{}

	Label() :
		Widget()
	{}
};

}}

#endif
