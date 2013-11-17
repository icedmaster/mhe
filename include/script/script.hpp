#ifndef __SCRIPT_HPP__
#define __SCRIPT_HPP__

namespace mhe {
namespace script {

class Script
{
public:
	virtual ~Script() {}
	virtual void call_method(const std::string& name) = 0;
};

}}

#endif
