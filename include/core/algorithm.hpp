#ifndef __ALGORITHM_HPP__
#define __ALGORITHM_HPP__

#include <algorithm> // temporary

namespace mhe {

template <class InputIterator, class T>
inline InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    return std::find<InputIterator, T>(first, last, value);
}

}

#endif
