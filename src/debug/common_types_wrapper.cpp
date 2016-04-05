#include "debug/common_types_wrapper.hpp"

#include "core/config.hpp"
#include "game/engine.hpp"
#include "render/instances.hpp"

#ifdef RDBG_ENABLED
#include "debug/rdbg.hpp"
#endif

#include "debug/serialization.hpp"

namespace mhe {

namespace {

size_t transform_instance_serialization_data_size()
{
    return 2 * sizeof(TransformInstance::IdType) + sizeof(vec3) + sizeof(quatf) + sizeof(vec3);
}

void serialize_transform_instance(uint8_t*& stream, const TransformInstance& transform_instance)
{
    serialize(stream, transform_instance.id);
    serialize(stream, transform_instance.parent);
    serialize(stream, transform_instance.transform.position());
    serialize(stream, transform_instance.transform.rotation());
    serialize(stream, transform_instance.transform.scale());
}

}

void register_common_types(RDBGProcessor& processor)
{
#ifdef RDBG_ENABLED
    TypeRegistrationInfo reginfo;
    reginfo.type_name = "transform";
    reginfo.set_data_func = transform_set_data;
    reginfo.get_data_func = transform_get_data;
    reginfo.set_object_data_func = transform_set_object_data;
    reginfo.get_object_data_func = transform_get_object_data;
    processor.register_type(reginfo);
#endif
}

bool transform_set_data(game::Engine& engine, const uint8_t* data, size_t size, size_t offset)
{
    NOT_IMPLEMENTED_METHOD();
    return false;
}

bool transform_get_data(game::Engine& engine, uint8_t* data, size_t& size, size_t offset)
{
    TransformInstance* xforms = engine.scene_context().transform_pool.all_objects();
    uint32_t xforms_number = engine.scene_context().transform_pool.size();
    ASSERT(xforms_number * transform_instance_serialization_data_size() + sizeof(uint32_t) < size, "Invalid serialization buffer size");
    size = xforms_number * transform_instance_serialization_data_size() + sizeof(uint32_t);
    serialize(data, xforms_number);
    for (size_t i = 0; i < xforms_number; ++i)
        serialize_transform_instance(data, xforms[i]);
    return true;
}

bool transform_set_object_data(game::Engine& engine, size_t objectid, const uint8_t* data, size_t size, size_t offset)
{
    return false;
}

bool transform_get_object_data(game::Engine& engine, size_t objectid, uint8_t* data, size_t& size, size_t offset)
{
    return false;
}

}
