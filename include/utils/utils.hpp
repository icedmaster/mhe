#ifndef _UTILS_HPP_
#define _UTILS_HPP_

namespace mhe
{
    namespace utils
    {
        template <class T>
        void truncate(T& v, const T& min_value, const T& max_value)
        {
            if (v < min_value) v = min_value;
            if (max_value < v) v = max_value;
        }
    }
}

#endif
