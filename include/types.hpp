#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <string>

namespace cmn
{
	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned long ulong;

	typedef signed char int8_t;
	typedef unsigned char uint8_t;
	typedef signed short int16_t;
	typedef unsigned short uint16_t;
	typedef signed int int32_t;
	typedef unsigned int uint32_t;
	typedef signed long long int64_t;
	typedef unsigned long long uint64_t;
};

namespace mhe
{
	class exception
	{
		private:
			std::string what_;
		public:
			exception(const std::string& es) {what_ = es;}
			const std::string& what() const {return what_;}
    };
}

#define nullptr 0

#endif
