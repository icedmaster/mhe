#include "res/serialize.hpp"

namespace mhe {
namespace res {

XMLSerializer::XMLSerializer(const char* filename, const char* node) : filename_(filename)
{
    pugi::xml_node mhe_node = doc_.append_child("mhe");
    node_ = mhe_node.append_child(node);
    current_node_ = node_;
}

XMLSerializer::~XMLSerializer()
{
    doc_.save_file(filename_.c_str());
}

bool XMLSerializer::write(const char* field, uint8_t value)
{
    return write(field, static_cast<uint32_t>(value));
}

bool XMLSerializer::write(const char* field, uint16_t value)
{
    return write(field, static_cast<uint32_t>(value));
}

bool XMLSerializer::write(const char* field, uint32_t value)
{
    pugi::xml_node node = current_node_.append_child(field);
    pugi::xml_attribute attr = node.append_attribute("value");
    attr.set_value(value);
    return true;
}

bool XMLSerializer::write(const char* field, float value)
{
    pugi::xml_node node = current_node_.append_child(field);
    pugi::xml_attribute attr = node.append_attribute("value");
    attr.set_value(value);
    return true;
}

bool XMLSerializer::write(const char* field, const string& value)
{
    pugi::xml_node node = current_node_.append_child(field);
    pugi::xml_attribute attr = node.append_attribute("value");
    if (!value.empty())
        attr.set_value(value.c_str());
    return true;
}

bool XMLSerializer::write(const char* field, const FilePath& value)
{
    pugi::xml_node node = current_node_.append_child(field);
    pugi::xml_attribute attr = node.append_attribute("ref");
    if (!value.empty())
        attr.set_value(value.c_str());
    return true;
}

void XMLSerializer::begin_field(const char* field)
{
    current_node_ = current_node_.append_child(field);
}

void XMLSerializer::end_field()
{
    current_node_ = current_node_.parent();
}

XMLDeserializer::XMLDeserializer(const char* filename, const char* node)
{
    pugi::xml_parse_result res = doc_.load_file(filename);
    if (res.status != pugi::status_ok) return;
    pugi::xml_node mhe_node = doc_.child("mhe");
    if (!mhe_node) return;
    current_node_ = mhe_node.child(node);
}

bool XMLDeserializer::read(const char* field, uint8_t& value)
{
    uint32_t t;
    bool res = read(field, t);
    value = t;
    return res;
}

bool XMLDeserializer::read(const char* field, uint16_t& value)
{
    uint32_t t;
    bool res = read(field, t);
    value = t;
    return res;
}

bool XMLDeserializer::read(const char* field, uint32_t& value)
{
    pugi::xml_node n = current_node_.child(field);
    if (!n) return false;
    pugi::xml_attribute attr = n.attribute("value");
    if (!attr) return false;
    value = attr.as_uint();
    return true;
}

bool XMLDeserializer::read(const char* field, float& value)
{
    pugi::xml_node n = current_node_.child(field);
    if (!n) return false;
    pugi::xml_attribute attr = n.attribute("value");
    if (!attr) return false;
    value = attr.as_float();
    return true;
}

bool XMLDeserializer::read(const char* field, string& value)
{
    pugi::xml_node n = current_node_.child(field);
    if (!n) return false;
    pugi::xml_attribute attr = n.attribute("value");
    if (!attr) return false;
    value = attr.value();
    return true;
}

bool XMLDeserializer::read(const char* field, FilePath& value)
{
    pugi::xml_node n = current_node_.child(field);
    if (!n) return false;
    pugi::xml_attribute attr = n.attribute("ref");
    if (!attr) return false;
    value = attr.value();
    return true;
}

void XMLDeserializer::begin_field(const char* field)
{
    current_node_ = current_node_.child(field);
}

void XMLDeserializer::end_field()
{
    current_node_ = current_node_.parent();
}

}}
