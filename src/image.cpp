#include "image.hpp"

void mhe::swapRB(Image& im)
{
	using namespace cmn;

	const cmn::uint bpp = im.bpp();
	if (bpp < 24) return;

	const cmn::uint w = im.width();
	const cmn::uint h = im.height();
	const std::vector<char>& d = im.get();

	std::vector<char> tmp;
	tmp.reserve(d.size());

	swapRB(bpp, d, tmp);

	im.set(tmp, w, h);
}

//
// For correct working, out must not contain any data
//
void mhe::swapRB(cmn::uint bpp, const std::vector<char>& in, std::vector<char>& out)
{
	if (bpp < 24) return;	// not supported

	for (size_t i = 0; i < in.size(); i += 3)
	{
		out.push_back(in[i + 2]);
		out.push_back(in[i + 1]);
		out.push_back(in[i]);
		if (bpp == 32)
		{
			out.push_back(in[i + 3]);
			++i;
		}
	}
}


void mhe::addAlphaChannel(Image& im, char a)
{
	using namespace cmn;

	const cmn::uint bpp = im.bpp();
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
			nd.insert(nd.end(), d.begin() + i, d.begin() + i + 3);
			nd.push_back(a);
		}
		im.set(nd, im.width(), im.height());
	}
}

void mhe::changeColorToAlpha(Image& im, const colorf& c, char a)
{
    // first, add alpha channel to image
    if (im.bpp() != 32)
        addAlphaChannel(im, 255);   //
    const colorf nc = c * 255;
    std::vector<char> d = im.get();
    for (size_t i = 0; i < d.size(); i += 4)
    {
        if ( (d[i] == nc.x()) && (d[i + 1] == nc.y()) && (d[i + 2] == nc.z()) )
            d[i + 3] = a;
    }
    im.set(d, im.width(), im.height());
}
