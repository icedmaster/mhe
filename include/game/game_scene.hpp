#ifndef __GAME_SCENE_HPP__
#define __GAME_SCENE_HPP__

#include "core/ref_counter.hpp"

namespace mhe {
namespace game {

class Engine;

struct GameSceneDesc
{
};

class GameScene : public ref_counter
{
public:
	virtual ~GameScene() {}
	
	virtual bool init(Engine& engine, const GameSceneDesc& desc) = 0;
	virtual void close() {}
	virtual bool update(Engine& engine) = 0;
	virtual void draw(const Engine& engine) {}
};

}}

#endif
