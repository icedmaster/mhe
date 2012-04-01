#ifndef __GAME_STONE_HPP__
#define __GAME_STONE_HPP__

#include "mhe.hpp"

class StoneFactory
{
public:
	StoneFactory(mhe::game::mhe_loader& loader, const std::map<int, std::string>& stones) :
		counter_(0), engine_(nullptr)
	{
		init(loader, stones);
	}

	boost::shared_ptr<mhe::game::Aspect> create_stone(int type, const mhe::v3d& pos) const;
	int get_random_type() const
	{
		return available_types_[mhe::utils::random(available_types_.size())];
	}
private:
	void init(mhe::game::mhe_loader& loader, const std::map<int, std::string>& stones);
	size_t get_next_id() const
	{
		const size_t max_id = 255;
		if (++counter_ >= max_id) counter_ = 0;
		return counter_;
	}

	typedef std::map< int, boost::shared_ptr<mhe::Sprite> > stone_sprite_map;
	stone_sprite_map sprites_;
	mutable size_t counter_;
	mhe::game::Engine* engine_;
	std::vector<int> available_types_;
};

class StoneEffectFactory
{
public:
	virtual ~StoneEffectFactory() {}
	virtual boost::shared_ptr<mhe::Node> create_move_stone_effect() const = 0;
	virtual boost::shared_ptr<mhe::Node> create_select_stone_effect() const = 0;
	virtual boost::shared_ptr<mhe::Node> create_remove_stone_effect() const = 0;
};

#endif
