#include "image.hpp"

void mhe::swapRB(Image& im)
{
	using namespace cmn;
	
	const uint bpp = im.bpp();
	if (bpp < 24) return;
	
	const uint w = im.width();
	const uint h = im.height();	
	const std::vector<char>& d = im.get();
	
	std::vector<char> tmp;
	tmp.reserve(d.size());
		
	for (size_t i = 0; i < d.size(); i += 3)
	{
		tmp.push_back(d[i + 2]);
		tmp.push_back(d[i + 1]);
		tmp.push_back(d[i]);
		if (bpp == 32)
		{
			tmp.push_back(d[i + 3]);
			++i;
		}
	}
	
	im.set(tmp, w, h);
}


void mhe::addAlphaChannel(Image& im, char a)	
{
	using namespace cmn;
	
	const uint bpp = im.bpp();
	if (bpp < 24) return;
	std::vector<char> d = im.get();
	
	if (bpp == 32)
	{
		for (size_t i = 0; i < d.size(); i += 4)
			d[i + 3] = a;
		im.set(d, im.width(), im.height());
	}
	else	// 24 bit
	{
		std::vector<char> nd;
		nd.reserve(d.size() + im.width() * im.height());	// additionaly, reserve space for alpha-channel
		for (size_t i = 0; i < d.size(); i += 3)
		{
			nd.assign(d.begin(), d.begin() + i + 3);
			nd.push_back(a);
		}
		im.set(nd, im.width(), im.height());
	}
}
