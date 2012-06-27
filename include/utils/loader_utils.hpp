#ifndef __LOADER_UTILS_HPP__
#define __LOADER_UTILS_HPP__

#include "lib/pugixml/pugixml.hpp"
#include "mhe_math.hpp"

namespace mhe {
namespace utils {

template <class T>
inline T read_number(const pugi::xml_node& node)
{
	std::wstring v = node.child_value();
	T n = 0;
	try
	{
		n = boost::lexical_cast<T>(v);
	}
	catch (std::exception&) {}
	return n;
}

template <class T>
inline T read_number(const pugi::xml_node& node,
					 const wstring& attr_name)
{
	std::wstring v = node.attribute(attr_name.c_str()).value();
	T n = 0;
	try
	{
		n = boost::lexical_cast<T>(v);
	}
	catch (exception&) {}
	return n;
}

inline vector3<float> read_vector3(const pugi::xml_node& n)
{
	float x = read_number(node, L"x");
	float y = read_number(node, L"y");
	float z = read_number(node, L"z");
	return vector3<float>(x, y, z);
}

inline vector4<float> read_vector4(const pugi::xml_node& n)
{
	float x = read_number(node, L"x");
	float y = read_number(node, L"y");
	float z = read_number(node, L"z");
	float w = read_number(node, L"w");
	return vector4<float>(x, y, z, w);
}

inline pugi::xml_node find_asset_with_type(const pugi::xml_node& root,
										   const std::wstring& type, const std::wstring& name)
{
	for (pugi::xml_node n = root.child(type); n; n = n.next_sibling(type)
	{
		if (n.attribute(L"name") == name)
			return n;
	}
	return pugi::xml_node();
}

}}

#endif
