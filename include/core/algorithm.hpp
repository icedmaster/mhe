#ifndef __ALGORITHM_HPP__
#define __ALGORITHM_HPP__

#include <algorithm> // temporary

namespace mhe {

template <class InputIterator, class T>
inline InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    return std::find<InputIterator, T>(first, last, value);
}

template<class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
	while (first!=last)
	{
		*result = *first;
		++result; ++first;
	}
	return result;
}

}

#endif
