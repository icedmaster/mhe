#ifndef __STATS_HPP__
#define __STATS_HPP__

#include "custom_types.hpp"
#include "core/types_cast.hpp"
#include "core/string.hpp"
#include "core/fixed_size_vector.hpp"
#include "core/allocator.hpp"

namespace mhe {

namespace game {

class Engine;

}

class Stats
{
public:
	struct Field
	{
		string name;
		int type;
		char value[24];
	};

	typedef fixed_capacity_vector<Field, 16> Fields;

	struct View
	{
		string name;
		Fields fields;
	};
public:
	Stats();

	size_t register_view(const char* name);
	size_t register_field(size_t stats, const char* name, int type);

	template <class T>
	void set_current_value(size_t view_id, size_t field_id, const T& value)
	{
		Field& field = views_[view_id].fields[field_id];
		TypeHelper<T>::serialize(field.value, sizeof(field.value), value);
	}

	void show_gui(size_t view) const;

	const View& view(size_t view_id) const
	{
		return views_[view_id];
	}

	size_t views_number() const
	{
		return views_.size();
	}
private:
	string to_string(const Field& field) const;

	typedef fixed_capacity_vector<View, 16> Views;

	Views views_;
};

void init_standart_stat_views(Stats& stats);
void update_standart_stats_views(Stats& stats, const game::Engine& engine);
MHE_EXPORT void show_standart_stats_views(Stats& stats);

}

#endif
