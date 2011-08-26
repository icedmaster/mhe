#include "mhe_sound_python_wrap.hpp"
#include <boost/python.hpp>

using namespace boost::python;

namespace
{
	void create_isound_wrap();
}

void mhe::wrap::create_sound_wrap()
{
	create_isound_wrap();
}

namespace
{
	void create_isound_wrap()
	{
		class iSoundWrap : public iSound, wrapper<iSound>
		{
			public:
				void play()
				{
					this->get_override("play");
				}
				
				void stop()
				{
					this->get_override("stop");
				}
		};
		
		class_<iSoundWrap>("iSound")
		.def("play", pure_virtual(&iSound::play))
		.def("stop", pure_virtual(&iSound::stop));
	}
}

BOOST_PYTHON_MODULE(_mhe_sound)
{
	mhe::wrap::create_sound_wrap();
}
