#ifndef __SHADER_PROGRAM_HPP__
#define __SHADER_PROGRAM_HPP__

#include <string>
#include <vector>
#include "core/unique_ptr.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/string.hpp"
#include "core/keyvalue.hpp"
#include "utils/pool_utils.hpp"

namespace mhe {

struct SamplerData
{
    string name;
    size_t index;
};

struct UniformData
{
    string name;
    size_t index;
};

struct ImageData
{
    string name;
    size_t index;
};

struct ShaderInitializationParams
{
    std::string vsdata;
    std::string gsdata;
    std::string fsdata;
    std::string csdata;
    std::string version;
    fixed_size_vector<SamplerData, 16> samplers;
    fixed_size_vector<UniformData, 16> uniforms;
    fixed_size_vector<SamplerData, 16> texture_buffers;
    fixed_size_vector<ImageData, 16> images;
    KeyValue<string, string> variables;
};

class ShaderProgramImpl
{
public:
    virtual ~ShaderProgramImpl() {}
    virtual bool init(const ShaderInitializationParams& params) = 0;
    virtual void close() = 0;
};

class MHE_EXPORT ShaderProgram
{
    friend class Driver;
    POOL_ELEMENT_METHODS(uint16_t);
public:
    ShaderProgram();
    bool init(const ShaderInitializationParams& params);
    void close()
    {
        impl_->close();
    }

    const ShaderProgramImpl* impl() const
    {
        return impl_.get();
    }

    template <class T>
    T variable_value(const string& key) const
    {
        return vars_.get<T>(key);
    }
private:
    unique_ptr<ShaderProgramImpl> impl_;
    KeyValue<string, string> vars_;
};

class MHE_EXPORT UberShader
{
    POOL_ELEMENT_METHODS(uint16_t);
public:
    struct Info
    {
        std::string name;
        size_t low;
        size_t high;
        size_t offset;

        Info() : offset(0) {}

        static Info empty()
        {
            static Info info;
            return info;
        }
    };

    class MHE_EXPORT Index
    {
    public:
        Index() :
                index_(0)
        {}

        void set(const Info& info, size_t value);
        size_t index() const
        {
            return index_;
        }

        void reset()
        {
            index_ = 0;
        }

        static size_t calculate_index(const Info& info, size_t value);
    private:
        size_t index_;
    };
public:
    void set_infos(const std::vector<Info>& infos)
    {
        infos_ = infos;
    }

    void add(size_t value, ShaderProgram::IdType id);
    ShaderProgram::IdType get(const Index& index) const;
    ShaderProgram::IdType get_default() const
    {
        return variants_[0];
    }

    const Info& info(const char* name) const;
private:
    std::vector<Info> infos_;
    std::vector<ShaderProgram::IdType> variants_;
};

// resource
struct Shader
{
    uint16_t shader_program_handle;
};

}

#endif
