#ifndef __GLOBAL_ENGINE_HPP__
#define __GLOBAL_ENGINE_HPP__

#include "engine.hpp"
#include "utils/singleton.hpp"

namespace mhe {
namespace game {

/// Helper class for game that using only one instance of Engine class
class GlobalEngine : public utils::Singleton<GlobalEngine>
{
	friend class utils::Singleton<GlobalEngine>;
public:
	void init(Engine* engine)
	{
		engine_ = engine;
	}

	Engine* get() const
	{
		return engine_;
	}
private:
	GlobalEngine() {}
	~GlobalEngine() {}

	Engine* engine_;
};

}}

#endif
