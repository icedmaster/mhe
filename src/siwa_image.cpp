#include "siwa_image.hpp"
#include <fstream>

namespace mhe
{
    bool siwa_image::load_impl(const std::string& fn)
    {
        std::ifstream f(fn.c_str(), std::ios::binary);
        if (!f.is_open()) return false;

        bool res = load_impl(f);
        f.close();

        return res;
    }

    bool siwa_image::load_impl(std::istream& stream)
    {
        // check image signature
        char dt[4];
        stream.read(dt, 4);
        if ( (dt[0] != 'S') || (dt[1] != 'I') ||
             (dt[2] != 'W') || (dt[3] != 'A') )
            return false;
        // get color, that should be converted to alpha
        stream.read(dt, 4);
        colorf clr(dt[0], dt[1], dt[2], dt[3]);
        clr /= 255;
        // read bmp image
        stream.seekg(0, std::ios::beg);
        stream.ignore(8);
        if (!bmp.load(stream))
            return false;
        changeColorToAlpha(bmp, clr, clr.a());
        return true;
    }
}
