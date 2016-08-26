#ifndef __SERIALIZE_HPP__
#define __SERIALIZE_HPP__

#include "core/string.hpp"
#include "core/vector.hpp"
#include "pugixml/pugixml.hpp"

namespace mhe {
namespace res {

class MHE_EXPORT Serializer
{
public:
    virtual ~Serializer() {}
    virtual bool write(const char* field, uint8_t value) = 0;
    virtual bool write(const char* field, uint16_t value) = 0;
    virtual bool write(const char* field, uint32_t value) = 0;
    virtual bool write(const char* field, float value) = 0;
    virtual bool write(const char* field, const string& value) = 0;
    virtual bool write(const char* field, const FilePath& value) = 0;
    virtual void begin_field(const char* field) = 0;
    virtual void end_field() = 0;

    template <class T>
    bool write(const char* field, const T& value)
    {
        begin_field(field);
        if (!value.write(*this)) return false;
        end_field();
        return true;
    }

    template <class T>
    bool write(const char* field, const vector<T>& value)
    {
        begin_field(field);
        if (!write("size", static_cast<uint32_t>(value.size()))) return false;
        for (size_t i = 0, size = value.size(); i < size; ++i)
        {
            if (!write("item", value[i]))
                return false;
        }
        end_field();
        
        return true;
    }

    template <class T>
    bool write_basic_type(const char* field, const T& value)
    {
        begin_field(field);
        if (!serialize(*this, value)) return false;
        end_field();
        return true;
    }
};

class MHE_EXPORT Deserializer
{
public:
    virtual ~Deserializer() {}
    virtual bool read(const char* field, uint8_t& value) = 0;
    virtual bool read(const char* field, uint16_t& value) = 0;
    virtual bool read(const char* field, uint32_t& value) = 0;
    virtual bool read(const char* field, float& value) = 0;
    virtual bool read(const char* field, string& value) = 0;
    virtual bool read(const char* field, FilePath& value) = 0;
    virtual void begin_field(const char* field) = 0;
    virtual void end_field() = 0;
    virtual void next_field(const char* field) = 0;

    virtual bool read(uint8_t& value) = 0;
    virtual bool read(uint16_t& value) = 0;
    virtual bool read(uint32_t& value) = 0;
    virtual bool read(float& value) = 0;
    virtual bool read(string& value) = 0;
    virtual bool read(FilePath& value) = 0;

    template <class T>
    bool read(const char* field, T& value)
    {
        begin_field(field);
        if (!value.read(*this)) return false;
        end_field();
        return true;
    }

    template <class T>
    bool read(T& value)
    {
        return value.read(*this);
    }

    template <class T>
    bool read(const char* field, vector<T>& value)
    {
        begin_field(field);
        uint32_t size;
        if (!read("size", size)) return false;
        value.resize(size);
        for (size_t i = 0, size = value.size(); i < size; ++i)
        {
            next_field("item");
            if (!read(value[i]))
                return false;
        }
        end_field();

        return true;
    }

    template <class T>
    bool read_basic_type(const char* field, T& value)
    {
        begin_field(field);
        if (!deserialize(*this, value)) return false;
        end_field();
        return true;
    }
};

class Serializable
{
public:
    virtual ~Serializable() {}
    virtual bool write(Serializer& serializer) const = 0;
    virtual bool read(Deserializer& serializer) = 0;
};

class MHE_EXPORT XMLSerializer : public Serializer
{
public:
    XMLSerializer(const char* filename, const char* node);
    ~XMLSerializer();

    bool write(const char* field, uint8_t value) override;
    bool write(const char* field, uint16_t value) override;
    bool write(const char* field, uint32_t value) override;
    bool write(const char* field, float value) override;
    bool write(const char* field, const string& value) override;
    bool write(const char* field, const FilePath& value) override;
    void begin_field(const char* field) override;
    void end_field() override;
private:
    FilePath filename_;
    pugi::xml_document doc_;
    pugi::xml_node node_;
    pugi::xml_node current_node_;
};

class MHE_EXPORT XMLDeserializer : public Deserializer
{
public:
    XMLDeserializer(const char* filename, const char* node);
    bool read(const char* field, uint8_t& value) override;
    bool read(const char* field, uint16_t& value) override;
    bool read(const char* field, uint32_t& value) override;
    bool read(const char* field, float& value) override;
    bool read(const char* field, string& value) override;
    bool read(const char* field, FilePath& value) override;

    bool read(uint8_t& value) override;
    bool read(uint16_t& value) override;
    bool read(uint32_t& value) override;
    bool read(float& value) override;
    bool read(string& value) override;
    bool read(FilePath& value) override;

    void begin_field(const char* field) override;
    void end_field() override;
    void next_field(const char* field) override;

    bool is_valid() const
    {
        return current_node_;
    }
private:
    pugi::xml_document doc_;
    pugi::xml_node current_node_;
};

}}

#define WRITE_FIELD(field, serializer) if (!serializer.write(#field, field)) return false;
#define WRITE_BASIC_TYPE_FIELD(field, serializer) if (!serializer.write_basic_type(#field, field)) return false;

#define READ_FIELD(field, serializer) if (!serializer.read(#field, field)) return false;
#define READ_FIELD_WITH_DEFAULT(field, serializer, default_value) if (!serializer.read(#field, field)) field = default_value;
#define READ_BASIC_TYPE_FIELD(field, serializer) if (!serializer.read_basic_type(#field, field)) return false;

#endif
