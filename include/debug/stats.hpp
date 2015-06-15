#ifndef __STATS_HPP__
#define __STATS_HPP__

#include "custom_types.hpp"
#include "core/types_cast.hpp"
#include "core/string.hpp"
#include "core/fixed_size_vector.hpp"

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
		string value;
	};

	typedef fixed_capacity_vector<Field, 16> Fields;

	struct View
	{
		string name;
		Fields fields;
	};
public:
	size_t register_view(const char* name);
	size_t register_field(size_t stats, const char* name, int type);

	template <class T>
	void set_current_value(size_t view, size_t field, const T& value)
	{
		views_[view].fields[field].value = types_cast<string>(value);
	}

	void prepare_for_rdbg(rdbg_string& str, size_t stats) const;
	void show_gui(size_t view) const;

	const View& view(size_t view_id) const
	{
		return views_[view_id];
	}
private:
	typedef fixed_capacity_vector<View, 16> Views;

	Views views_;
};

void init_standart_stat_views(Stats& stats);
void update_standart_stats_views(Stats& stats, const game::Engine& engine);
MHE_EXPORT void show_standart_stats_views(Stats& stats);

}

#endif
