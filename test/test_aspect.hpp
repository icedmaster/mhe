#ifndef __TEST_ASPECT_HPP__
#define __TEST_APSECT_HPP__

#include "test_game_common.hpp"

class TestLogicAspect : public mhe::game::LogicAspect
{
public:
	TestLogicAspect(const std::string& name) :
		mhe::game::LogicAspect(name), prev_(0)
	{}
private:
	void do_subscribe(mhe::game::Aspect* parent)
	{ parent->subscribe(mhe::game::TickEvent, this); }

	void update_impl(int type, const void* prm)
	{
		if (type != mhe::game::TickEvent) return;
		cmn::uint cur = *(static_cast<cmn::uint*>(prm));
		if (!prev_) prev_ = cur;
		else if (cur >= (prev_ + 1000))
		{
			update(42, 0);	
			prev_ = cur;
		}
	}

	cmn::uint prev_;
};

class PrintAspect : public mhe::game::Aspect
{
public:
	PrintAspect(const std::string& name) : Aspect(name) {}
private:
	void do_subscribe(mhe::game::Aspect* parent)
	{
		parent->subscribe(42, this);
	}

	void update_impl(int type, const void*)
	{
		std::cout << "test\n";
	}
};

class TestAspect : public mhe::test::TestGameCommon
{
protected:
	struct UpdateCallback : public mhe::Scene::Callback
	{
		UpdateCallback(mhe::game::Aspect* top) : top_(top) {}
		void beforeDraw(const mhe::Scene*, boost::shared_ptr<mhe::iDriver>)
		{
			cmn::uint now = mhe::utils::get_current_tick();
			top_->update(mhe::game::TickEvent, &now);
		}
		mhe::game::Aspect* top_;
	};

	boost::shared_ptr<mhe::game::Aspect> test_aspect;
	void SetUp()
	{
		mhe::test::TestGameCommon::SetUp();
		test_aspect.reset(new TestLogicAspect("a1"));
		test_aspect->attach(new PrintAspect("pa"));
		engine.get_game_scene()->getScene()->setCallback(new UpdateCallback(test_aspect.get()));
	}
};

TEST_F(TestAspect, test_base)
{
	engine.run();
}

#endif
