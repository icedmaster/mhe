#ifndef __NONCOPYABLE_HPP__
#define __NONCOPYABLE_HPP__

// same as boost/noncopyable

namespace mhe {
namespace utils {

class noncopyable
{
protected:
	noncopyable() {}
	~noncopyable() {}
private:
	noncopyable(const noncopyable&);
	noncopyable& operator= (const noncopyable&);
};

}}

#endif