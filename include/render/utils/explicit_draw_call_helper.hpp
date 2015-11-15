#ifndef __EXPLICIT_DRAW_CALL_HELPER_HPP__
#define __EXPLICIT_DRAW_CALL_HELPER_HPP__

#include "core/compiler.hpp"

namespace mhe {

struct Context;
struct DrawCall;
struct DrawCallExplicit;

namespace utils {

MHE_EXPORT void convert(DrawCallExplicit& dst, const DrawCall& src, const Context& context);

}}

#endif
