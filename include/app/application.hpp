#ifndef __APPLICATION_HPP__ 
#define __APPLICATION_HPP__

#include "application_config.hpp"
#include "core/ref_counter.hpp"
#include "game/engine.hpp"

namespace mhe {
namespace app {

class MHE_EXPORT Application : public ref_counter
{
public:
	Application(const std::string& name = "");
	virtual ~Application() {}
	bool init(const ApplicationConfig& config);
	void close();

	int run()
	{
		return run_impl();
	}

    void stop()
    {
        stop_impl();
    }

	const std::string& name() const
	{
		return name_;
	}

	std::string path() const
	{
		return application_base_path();
	}

	game::Engine& engine()
	{
		return engine_;
	}
protected:
	// methods with default implementation
	virtual bool mhe_app_init(const ApplicationConfig& config);
	virtual void mhe_app_close();
	virtual int run_impl();
    virtual void stop_impl();
	virtual void init_assets_path(const std::string& config_assets_path);
	virtual void init_default_assets(const ApplicationConfig& config);

	virtual std::string application_base_path() const
	{
		return "";
	}
private:
	virtual void init_impl() {}
	virtual void close_impl() {}

    void add_delegates();
    bool on_system_event(const Event* event);

	game::Engine engine_;
	std::string name_;
};

}}

#endif
