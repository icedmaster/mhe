#ifndef __OPENGL3_QUERY_HPP__
#define __OPENGL3_QUERY_HPP__

#include "render/query.hpp"
#include "platform/opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class OpenGL3Query : public QueryImpl
{
public:
	bool init(int target) override;
	void destroy() override;
	void begin() override;
	void end() override;
	void get(int& res) const override;
	void get(uint64_t& res) const override;
	void set() override;
private:
	GLuint id_;
	GLenum target_;
};

}}

#endif
