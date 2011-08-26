#ifndef _IFONT_HPP_
#define _IFONT_HPP_

#include <string>
#include "idriver.hpp"
#include "mhe_math.hpp"

namespace mhe
{
	namespace gui
	{
		class iFont
		{
			public:
				virtual ~iFont() {}

				virtual void print(const boost::shared_ptr<iDriver>& driver,
                                   const std::wstring& text,
                                   const v2d& coord) = 0;
				virtual void setColor(const colorf& color) = 0;

				virtual cmn::uint height() const = 0;
				virtual std::string name() const = 0;

				virtual iFont* clone() const = 0;
		};
	}
}

#endif
