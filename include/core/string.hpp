#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <cstring>
#include <string>
#include "assert.hpp"

namespace mhe {

template <class T, size_t S>
class basic_string
{
public:
	typedef T value_type;
	typedef basic_string<T, S> this_type;
	typedef T* iterator;
	typedef const T* const_iterator;
	
	static const size_t npos = static_cast<size_t>(-1);
public:
	basic_string() :
		size_(0)
	{}

	explicit basic_string(const T* str)
	{
		assign(str, strlen(str));
	}

	basic_string(const T* str, size_t len)
	{
		assign(str, len);
	}

	basic_string(const this_type& str) :
		size_(str.size_)
	{
		::memcpy(str_, str.str_, size_);
		str_[size_] = 0;
	}

	template <size_t N>
	basic_string(const basic_string<value_type, N>& str)
	{
		assign(str.data(), str.length());
	}

    template <class Str>
    basic_string(const Str& s)
	{
		assign(s, strlen(s));
	}

	basic_string(const std::basic_string<T>& str)
	{
		assign(str.c_str(), str.length());
	}

	this_type& operator= (const this_type& other)
	{
		if (this == &other) return *this;
		assign(other.str_, other.size_);
		return *this;
	}

	this_type& operator= (const T* str)
	{
		assign(str, strlen(str));
		return *this;
	}

	this_type& operator= (const std::basic_string<T>& str)
	{
		if (this == &str) return *this;
		assign(str.c_str(), str.length());
		return *this;
	}

	this_type& operator= (T s)
	{
		assign(s);
		return *this;
	}

	template <class Str>
	this_type& operator= (const Str& str)
	{
		static_assert(sizeof(T) == sizeof(Str::value_type), "Invalid type");
		assign(str.data(), str.length());
		return *this;
	}

	bool operator== (const this_type& other) const
	{
		return operator==<this_type>(other);
	}

	bool operator!= (const this_type& other) const
	{
		return !operator==(other);
	}

    template <size_t Size>
    bool operator== (const basic_string<T, Size>& other) const
	{
		if (size_ != other.size()) return false;
		const T* t1 = str_;
        const T* t2 = other.data();
		return ::memcmp(t1, t2, size_ * sizeof(T)) == 0;
	}

	bool operator== (const T* s) const
	{
		if (size_ != strlen(s)) return false;
		return ::memcmp(str_, s, size_) == 0;
	}

	template <class Str>
	bool operator!= (const Str& other) const
	{
		return !operator==<Str>(other);
	}

	T operator[] (size_t index) const
	{
		ASSERT(index < size_, "The index is too big");
		return str_[index];
	}

	this_type& operator+= (const this_type& s)
	{
		append(s);
		return *this;
	}

	template <class Str>
	this_type& operator+= (const Str& s)
	{
		append<Str>(s);
		return *this;
	} 

	this_type& operator+= (const T* s)
	{
		append(s);
		return *this;
	}

	this_type& operator+= (T s)
	{
		append(s);
		return *this;
	}

    bool operator< (const this_type& s) const
    {
        if (s.size_ != size_) return s.size_ < size_;
        for (size_t i = 0; i < size_; ++i)
        {
            if (str_[i] == s.str_[i]) continue;
            return str_[i] < s.str_[i];
        }
        return false;
    }

	size_t length() const
	{
		return size_;
	}

	size_t size() const
	{
		return size_;
	}

	size_t capacity() const
	{
		return S;
	}

	const T* data() const
	{
		return str_;
	}

	const T* c_str() const
	{
		return str_;
	}

	T* buffer()
	{
		return str_;
	}

	iterator begin()
	{
		return str_;
	}

	const_iterator begin() const
	{
		return str_;
	}

	iterator end()
	{
		return str_ + size_;
	}

	const_iterator end() const
	{
		return str_ + size_;
	}

	bool empty() const
	{
		return size_ == 0;
	}

	size_t find(T s, size_t pos = 0) const
	{
		for (size_t i = pos; i < size_; ++i)
		{
			if (str_[i] == s) return i;
		}
		return npos;
	} 

	template <class Str>
	size_t find(const Str& s, size_t pos = 0) const
	{
		static_assert(sizeof(T) == sizeof(Str::value_type), "Invalid type");
		return find(s.data(), s.length(), pos);
	}

	size_t find(const T* data, size_t pos = 0) const
	{
		return find(data, strlen(data), pos);
	}

	size_t find(const this_type& s, size_t pos = 0) const
	{
		return find<this_type>(s, pos);
	}

	size_t rfind(T s, size_t pos = npos) const
	{
		if (size_ == 0) return npos;
		int start = pos == npos ? size_ - 1 : pos;
		// prevent warning
		for (int i = start; i >=0; --i)
		{
			if (str_[i] == s) return i;
		}
		return npos;
	}

	template <class Str>
	size_t rfind(const Str& s, size_t pos = npos) const
	{
		static_assert(sizeof(T) == sizeof(Str::value_type), "Invalid type");
		const typename Str::value_type* other_data = s.data();
		size_t other_length = s.length();
		size_t start = pos == npos ? size_ : pos;
		if (start - other_length > size_) return npos;
		for (int i = start - other_length; i >= 0; --i)
		{
			if (str_[i] == other_data[0])
			{
				if (::memcmp(str_ + i, other_data, other_length) == 0)
					return i;
			}
		}
		return npos;	
	}

	size_t rfind(const this_type& s, size_t pos = npos) const
	{
		return rfind<this_type>(s, pos);
	}

	void append(T s)
	{
		ASSERT(size_ < S - 2, "Can't append a new symbol");
		str_[size_++] = s;
		str_[size_] = 0;
	}

	template <class Str>
	void append(const Str& s)
	{
        static_assert(sizeof(T) == sizeof(typename Str::value_type), "Invalid type");
		append(s.data(), s.length());
	}

	void append(const T* s)
	{
		append(s, strlen(s));
	}

	void erase(size_t index = 0, size_t length = npos)
	{
		size_t len = length == npos ? size_ - index : length;
		ASSERT(index + len <= size_, "Invalid erase parameters");
		if (index + len != size_)
			memmove(str_ + index, str_ + index + len, size_ - index - len);
		size_ -= len;
	}

	void clear()
	{
		str_[0] = 0;
		size_ = 0;
	}

	this_type substr(size_t index, size_t length) const
	{
		ASSERT(index + length <= size_, "Invalid substr parameters");
		return this_type(str_ + index, length);
	}

	size_t find_first_not_of(const this_type& s) const
	{
		return find_first_not_of(s.str_, s.size_);
	}

	template <class Str>
	size_t find_first_not_of(const Str& s) const
	{
		static_assert(sizeof(T) == sizeof(Str::value_type), "Invalid type");
		return find_first_not_of(s.data(), s.length());
	}

	size_t find_first_not_of(const T* s) const
	{
		return find_first_not_of(s, strlen(s));
	}

	size_t find_first_not_of(T s) const
	{
		for (size_t i = 0; i < size_; ++i)
		{
			if (str_[i] != s) return i;
		}
		return npos;
	}

	size_t find_last_not_of(const this_type& s) const
	{
		return find_last_not_of(s.str_, s.size_);
	}

	template <class Str>
	size_t find_last_not_of(const Str& s) const
	{
		return find_last_not_of(s.data(), s.length());
	}

	size_t find_last_not_of(const T* s) const
	{
		return find_last_not_of(s, strlen(s));
	}

	size_t find_last_not_of(T s) const
	{
		for (int i = size_ - 1; i >= 0; --i)
		{
			if (str_[i] != s) return i;
		}
		return npos;
	}

	size_t find_first_of(const this_type& s, size_t pos = 0) const
	{
		return find_first_of(s.str_, s.size_, pos);
	}

	template <class Str>
	size_t find_first_of(const Str& s, size_t pos = 0) const
	{
		return find_first_of(s.data(), s.length(), pos);
	}

	size_t find_first_of(const T* s, size_t pos = 0) const
	{
		return find_first_of(s, strlen(s), pos);
	}

	size_t find_first_of(T s, size_t pos = 0) const
	{
		for (int i = pos; i < size_; ++i)
		{
			if (str_[i] == s) return i;
		}
		return npos;
	}

    size_t find_last_of(T s, size_t pos = npos) const
    {
        size_t last = pos == npos ? size_ - 1 : pos;
        for (int i = last; i >= 0; --i)
        {
            if (str_[i] == s) return i;
        }
        return npos;
    }

	// adds trailing 0
	void make_cstr()
	{
		str_[size_] = 0;
	}

	void resize(size_t size)
	{
		size_ = size;
		make_cstr();
	}
private:
	void assign(const T* str, size_t len)
	{
		ASSERT(len < S - 1, "String is too big");
		size_ = len;
		::memcpy(str_, str, size_);
		str_[size_] = 0;
	}

	void assign(T s)
	{
        static_assert(S >= 2, "Invalid string size");
		str_[0] = s;
		str_[1] = 0;
		size_ = 1;
	}

	void append(const T* s, size_t len)
	{
		ASSERT(size_ + len + 1 < S, "Can't append a string");
		::memcpy(str_ + size_, s, len);
		size_ += len;
		str_[size_] = 0;
	}

	size_t find(const T* s, size_t len, size_t pos) const
	{
		if (pos + len > size_) return npos;
		for (size_t i = pos; i <= size_ - len; ++i)
		{
			if (str_[i] == s[0])
			{
				if (::memcmp(str_ + i, s , len) == 0)
					return i;
			}
		}
		return npos;
	}

	size_t find_first_not_of(const T* data, size_t len) const
	{
		for (size_t i = 0; i < size_; ++i)
		{
			bool found = false;
			for (size_t j = 0; j < len; ++j)
			{
				if (data[j] == str_[i])
				{
					found = true;
					break;
				}
			}
			if (found) continue;
			return i;
		}
		return npos;
	}

	size_t find_first_of(const T* data, size_t len, size_t index) const
	{
		for (size_t i = index; i < size_; ++i)
		{
			for (size_t j = 0; j < len; ++j)
			{
				if (data[j] == str_[i])
					return i;
			}
		}
		return npos;
	}

	size_t find_last_not_of(const T* data, size_t len) const
	{
		for (int i = size_ - 1; i >= 0; --i)
		{
			bool found = false;
			for (size_t j = 0; j < len; ++j)
			{
				if (data[j] == str_[i])
				{
					found = true;
					break;
				}
			}
			if (found) continue;
			return i;
		}
		return npos;
	}

	T str_[S];
	size_t size_;
};

template <class T, size_t S1, size_t S2>
basic_string<T, S1> operator+ (const basic_string<T, S1>& str1, const basic_string<T, S2>& str2)
{
    basic_string<T, S1> s(str1);
	s += str2;
	return s;
}

template <class T, size_t S>
basic_string<T, S> operator+ (char s1[], const basic_string<T, S>& str2)
{
    return basic_string<T, S>(s1) + str2;
}

template <class T, size_t S>
basic_string<T, S> operator+ (const basic_string<T, S>& s1, char s2[])
{
    return s1 + basic_string<T, S>(s2);
}

template <class T, size_t S>
inline std::ostream& operator<< (std::ostream& stream, const basic_string<T, S>& str)
{
	stream << str.data();
	return stream;
}

const size_t default_string_capacity = 64;
const size_t default_filepath_capacity = 255;

typedef basic_string<char, default_string_capacity> string;
typedef basic_string<char, default_filepath_capacity> FilePath;

}

#endif
