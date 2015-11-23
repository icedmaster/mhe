#ifndef __RDBG_HPP__
#define __RDBG_HPP__

#include <string>
#include <vector>
#include <map>
#include "custom_types.hpp"
#include "core/hashmap.hpp"
#include "core/singleton.hpp"
#include "core/types_cast.hpp"
#include "core/string.hpp"
#include "core/vector.hpp"
#include "threads/thread.hpp"
#include "net/socklib.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"

namespace mhe {

namespace game {

class Engine;

}

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

typedef fixed_size_vector<char, 1024> rdbgvector;

struct RDBGData
{
	rdbgvector data;
	uint8_t cmd;

	RDBGData() {}
	RDBGData(uint8_t res) : cmd(res) {}
};

class ByteStream
{
public:
	ByteStream(const char* data, size_t size) : data_(data), size_(size), pos_(0) {}

	const char* read(size_t n)
	{
		const char* res = data_ + pos_;
		pos_ += n;
		return res;
	}
private:
	const char* data_;
	size_t size_;
	size_t pos_;
};

class GlobalVars : public Singleton<GlobalVars>
{
	friend class Singleton<GlobalVars>;
public:
	struct Data
	{
		const char* name;
		int type;
		string value;
		bool changed;
	};
public:
	template <class T>
	size_t add(const char* name, const T& value)
	{
		size_t index = vars_.size();
		set(name, value);
		return index;
	}

	template <class T>
	T value(const char* name) const
	{
		NameToVectorMap::const_iterator it = names_indices_.find(string(name));
		if (it == vars_.end()) return T();
		return types_cast<T>(vars_[it->value].value);
	}

	template <class T>
	T value(size_t index) const
	{
		return types_cast<T>(vars_[index].value);
	}

	template <class T>
	void set(const char* name, const T& value)
	{
		NameToVectorMap::iterator it = names_indices_.find(string(name));
		if (it != names_indices_.end())
		{
			Data& data = vars_[it->value];
			data.value = types_cast<string>(value);
		}
		else
		{
			size_t index = vars_.size();
			Data data;
			data.name = name;
			data.type = TypeHelper<T>::type;
			data.value = types_cast<string>(value);
			data.changed = true;
			vars_.push_back(data);
			names_indices_[string(name)] = index;
		}
	}

	template <class T>
	void set(size_t index, const T& value)
	{
		vars_[index] = types_cast<string>(value);
	}

	void set(const string& name, ByteStream& stream);

	bool has(const char* name) const
	{
		return names_indices_.find(string(name)) != names_indices_.end();
	}

	bool reset_if_changed(const char* name)
	{
		NameToVectorMap::iterator it = names_indices_.find(string(name));
		if (it == names_indices_.end()) return false;
		return reset_if_changed(it->value);
	}

	bool reset_if_changed(size_t index)
	{
		Data& data = vars_[index];
		bool changed = data.changed;
		data.changed = false;
		return changed;
	}

	const vector<Data>& data() const;

	void update_data(size_t index, const string& data)
	{
		vars_[index].value = data;
		vars_[index].changed = true;
	}
private:
	GlobalVars() {}
	~GlobalVars() {}

	typedef vector<Data> VarsVector;
	typedef hashmap<string, size_t> NameToVectorMap;
	VarsVector vars_;
	NameToVectorMap names_indices_;
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

	bool init();

	void register_type(const Data& data)
	{
		data_[hash(data.name)] = data;
	}

	bool type_registrated(const std::string& name) const
	{
		return data_.find(hash(name)) != data_.end();
	}

	RDBGData process_command(const RDBGData& cmd);

	void update();
private:
	RDBGData make_error(const char* message) const;

	//RDBGData set_data(Data& data, size_t id, const std::vector<std::string>& subtypes, const std::string* args, size_t argc);
	//RDBGData get_data(Data& data, size_t id, const std::vector<std::string>& subtypes);

	bool set(const Data& data, size_t id, int value)
	{
		return data.set.set_int(engine_, id, value);
	}

	bool set(const Data& data, size_t id, const vec4& value)
	{
		return data.set.set_vec4(engine_, id, value);
	}

	bool process_default_command(RDBGData& result, const RDBGData& data);
	bool process_var(RDBGData& result, const RDBGData& cmd);
	RDBGData process_get_all_command(const RDBGData& data);
	RDBGData process_profiler_result_command(const RDBGData& data);
	RDBGData process_stats_command(const RDBGData& data);
	//void process_get_data(std::string& result, const Data& data) const;

	//const Data* find_data(const Data& root, const std::vector<std::string>& names) const;

	bool set_var(RDBGData& result, const RDBGData& cmd);
	bool get_var(RDBGData& result, const RDBGData& cmd);

	void update_variables_data();
	void update_profiler_data();
	void update_gpu_profiler_data();
	void update_stats_data();

	typedef std::map<hash_type, Data> DataMap;
	DataMap data_;
	game::Engine& engine_;
	vector<GlobalVars::Data> global_vars_copy_;
	rdbgvector all_variables_;
	rdbgvector profiler_data_;
	rdbgvector gpu_profiler_data_;
	rdbgvector stats_data_;
	mutex mutex_;
};

class RDBGThread : public thread
{
public:
	RDBGThread(RDBGProcessor& processor) :
			processor_(processor)
	{}
private:
	bool start_impl();
	void process_impl();

	RDBGProcessor& processor_;
	net::tcp_socket server_socket_;
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

	bool start();
	void stop();

	void update();
private:
	RDBGProcessor processor_;
	RDBGThread thread_;
};

template <class T>
class GlobalVar
{
public:
	GlobalVar() {}

	GlobalVar(const char* name, const T& val) :
		name_(name)
	{
		ASSERT(GlobalVars::instance().has(name) == false, "Attempt to add global variable twice");
		index_ = GlobalVars::instance().add(name, val);
	}

	GlobalVar(const char* name) :
		name_(name)
	{
		if (!GlobalVars::instance().has(name))
			index_ = GlobalVars::instance().add(name, T());
	}

	T value() const
	{
		return GlobalVars::instance().value<T>(index_);
	}

	operator T()
	{
		return value();
	}

	GlobalVar& operator= (const T& val)
	{
		GlobalVars::instance().set(index_, val);
	}

	GlobalVar& operator= (const GlobalVar& other)
	{
		name_ = other.name_;
		index_ = other.index_;
		return *this;
	}

	bool reset_if_changed()
	{
		return GlobalVars::instance().reset_if_changed(index_);
	}
private:
	const char* name_;
	size_t index_;
};

}

#endif
