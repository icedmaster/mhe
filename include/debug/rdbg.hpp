#ifndef __RDBG_HPP__
#define __RDBG_HPP__

#include <string>
#include <vector>
#include <map>
#include "core/hash.hpp"
#include "threads/thread.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"

namespace mhe {

namespace game {

class Engine;

}

enum
{
	None,
	Int,
	Float,
	String,
	Vector3,
	Vector4
};

typedef bool (*SetFuncInt)(game::Engine& engine, size_t id, int value);
typedef bool (*GetFuncInt)(game::Engine& engine, size_t id, int& value);

typedef bool (*SetFuncFloat)(game::Engine& engine, size_t id, float value);
typedef bool (*GetFuncFloat)(game::Engine& engine, size_t id, float& value);

typedef bool (*SetFuncString)(game::Engine& engine, size_t id, const char* value);
typedef bool (*GetFuncString)(game::Engine& engine, size_t id, char* value);

typedef bool (*SetFuncVector3)(game::Engine& engine, size_t id, const vec3& value);
typedef bool (*GetFuncVector3)(game::Engine& engine, size_t id, vec3& value);

typedef bool (*SetFuncVector4)(game::Engine& engine, size_t id, const vec4& value);
typedef bool (*GetFuncVector4)(game::Engine& engine, size_t id, vec4& value);

union SetFunc
{
	SetFuncInt set_int;
	SetFuncFloat set_float;
	SetFuncString set_string;
	SetFuncVector3 set_vec3;
	SetFuncVector4 set_vec4;
};

union GetFunc
{
	GetFuncInt get_int;
	GetFuncFloat get_float;
	GetFuncString get_string;
	GetFuncVector3 get_vec3;
	GetFuncVector4 get_vec4;
};

class RDBGProcessor
{
public:
	struct Data
	{
		std::vector<Data> children;
		std::string name;
		SetFunc set;
		GetFunc get;
		int type;

		Data() :
			type(None)
		{}

		Data(const char* data_name) :
			name(data_name),
			type(None)
		{}

		template <class SF, class GF>
		Data& attach(const char* name, int type, SF set_func, GF get_func)
		{
			Data child;
			child.name = name;
			child.type = type;
			update_func(child, set_func, get_func);
			children.push_back(child);
			return children.back();
		}

		void update_func(Data& child, SetFuncInt set_func, GetFuncInt get_func)
		{
			child.set.set_int = set_func;
			child.get.get_int = get_func;
		}

		void update_func(Data& child, SetFuncFloat set_func, GetFuncFloat get_func)
		{
			child.set.set_float = set_func;
			child.get.get_float = get_func;
		}

		void update_func(Data& child, SetFuncString set_func, GetFuncString get_func)
		{
			child.set.set_string = set_func;
			child.get.get_string = get_func;
		}

		void update_func(Data& child, SetFuncVector3 set_func, GetFuncVector3 get_func)
		{
			child.set.set_vec3 = set_func;
			child.get.get_vec3 = get_func;
		}

		void update_func(Data& child, SetFuncVector4 set_func, GetFuncVector4 get_func)
		{
			child.set.set_vec4 = set_func;
			child.get.get_vec4 = get_func;
		}
	};
public:
	RDBGProcessor(game::Engine& engine) :
		engine_(engine)
	{}

	void register_type(const Data& data)
	{
		data_[hash(data.name)] = data;
	}

	bool type_registrated(const std::string& name) const
	{
		return data_.find(hash(name)) != data_.end();
	}

	std::string process_command(const std::string& cmd);
private:
	std::string make_error(const char* message) const;

	std::string set_data(Data& data, size_t id, const std::string* args, size_t argc);
	std::string get_data(Data& data, size_t id);

	bool set(Data& data, size_t id, int value)
	{
		return data.set.set_int(engine_, id, value);
	}

	bool set(Data& data, size_t id, const vec4& value)
	{
		return data.set.set_vec4(engine_, id, value);
	}

	typedef std::map<hash_type, Data> DataMap;
	DataMap data_;
	game::Engine& engine_;
};

class RDBGThread : public thread
{
public:
	RDBGThread(RDBGProcessor& processor) :
			processor_(processor)
	{}
private:
	void process_impl();

	RDBGProcessor processor_;
};

class RDBGEngine
{
public:
	RDBGEngine(game::Engine& engine) :
			processor_(engine),
			thread_(processor_)
	{}

	RDBGProcessor& processor()
	{
		return processor_;
	}
private:
	RDBGProcessor processor_;
	RDBGThread thread_;
};

}

#endif
