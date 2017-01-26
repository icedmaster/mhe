#ifndef __OPENGL3_ATOMICS_HPP__
#define __OPENGL3_ATOMICS_HPP__

#include "render/atomics.hpp"
#include "opengl3_buffer.hpp"

namespace mhe {
namespace opengl {

class OpenGL3Atomic : public AtomicImpl
{
public:
    bool init(int type) override;
    void destroy() override;
    void update(const uint8_t* value, size_t size) override;
    void bind(size_t unit) const;
private:
    size_t get_size_by_type(int type) const;

    VBO vbo_;
};

}}

#endif
