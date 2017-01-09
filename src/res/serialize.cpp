#include "res/serialize.hpp"

namespace mhe {
namespace res {

XMLSerializer::XMLSerializer(const char* filename, const char* node) : filename_(filename)
{
    pugi::xml_node mhe_node = doc_.append_child("mhe");
    node_ = mhe_node.append_child(node);
    current_node_ = node_;
}

XMLSerializer::XMLSerializer(const char* filename) : filename_(filename)
{
    pugi::xml_node mhe_node = doc_.append_child("mhe");
    current_node_ = mhe_node;
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
    pugi::xml_attribute attr = node.append_attribute("v");
    attr.set_value(value);
    return true;
}

bool XMLSerializer::write(const char* field, uint64_t value)
{
    return write(field, static_cast<uint32_t>(value));
}


bool XMLSerializer::write(const char* field, float value)
{
    pugi::xml_node node = current_node_.append_child(field);
    pugi::xml_attribute attr = node.append_attribute("v");
    attr.set_value(value);
    return true;
}

bool XMLSerializer::write(const char* field, const string& value)
{
    pugi::xml_node node = current_node_.append_child(field);
    pugi::xml_attribute attr = node.append_attribute("v");
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

XMLDeserializer::XMLDeserializer(const char* filename)
{
    pugi::xml_parse_result res = doc_.load_file(filename);
    if (res.status != pugi::status_ok) return;
    pugi::xml_node mhe_node = doc_.child("mhe");
    ASSERT(mhe_node, "Invalid xml file");
    current_node_ = mhe_node;
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
    pugi::xml_attribute attr = n.attribute("v");
    if (!attr) return false;
    value = attr.as_uint();
    return true;
}

bool XMLDeserializer::read(const char* field, uint64_t& value)
{
    uint32_t t;
    bool res = read(field, t);
    value = t;
    return res;
}

bool XMLDeserializer::read(const char* field, float& value)
{
    pugi::xml_node n = current_node_.child(field);
    if (!n) return false;
    pugi::xml_attribute attr = n.attribute("v");
    if (!attr) return false;
    value = attr.as_float();
    return true;
}

bool XMLDeserializer::read(const char* field, string& value)
{
    pugi::xml_node n = current_node_.child(field);
    if (!n) return false;
    pugi::xml_attribute attr = n.attribute("v");
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

bool XMLDeserializer::read(uint8_t& value)
{
    uint32_t t;
    if (!read(t)) return false;
    value = t;
    return true;
}

bool XMLDeserializer::read(uint16_t& value)
{
    uint32_t t;
    if (!read(t)) return false;
    value = t;
    return true;
}

bool XMLDeserializer::read(uint32_t& value)
{
    pugi::xml_attribute attr = current_node_.attribute("v");
    if (!attr) return false;
    value = attr.as_uint();
    return true;
}

bool XMLDeserializer::read(float& value)
{
    pugi::xml_attribute attr = current_node_.attribute("v");
    if (!attr) return false;
    value = attr.as_float();
    return true;
}

bool XMLDeserializer::read(string& value)
{
    pugi::xml_attribute attr = current_node_.attribute("v");
    if (!attr) return false;
    value = attr.value();
    return true;
}

bool XMLDeserializer::read(FilePath& value)
{
    pugi::xml_attribute attr = current_node_.attribute("ref");
    if (!attr) return false;
    value = attr.value();
    return true;
}

bool XMLDeserializer::begin_field(const char* field)
{
    pugi::xml_node n = current_node_.child(field);
    if (!n) return false;
    current_node_ = n;
    return true;
}

void XMLDeserializer::end_field()
{
    current_node_ = current_node_.parent();
}

void XMLDeserializer::next_field(const char* field)
{
    pugi::xml_node prev_node = current_node_;
    current_node_ = current_node_.next_sibling(field);
    if (!current_node_) current_node_ = prev_node.child(field);
    if (!current_node_) current_node_ = prev_node;
}

}}
