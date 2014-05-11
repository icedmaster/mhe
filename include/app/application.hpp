#ifndef __APPLICATION_HPP__ 
#define __APPLICATION_HPP__

#include "application_config.hpp"
#include "core/ref_counter.hpp"

namespace mhe {
namespace app {

class Application : public ref_counter
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

	const std::string& name() const
	{
		return name_;
	}

	std::string path() const
	{
		return application_base_path();
	}
protected:
	// methods with default implementation
	virtual bool mhe_app_init(const ApplicationConfig& config);
	virtual void mhe_app_close();
	virtual int run_impl();
	virtual void init_assets_path(const std::string& config_assets_path);
	virtual void init_default_assets(const ApplicationConfig& config);

	virtual std::string application_base_path() const
	{
		return "";
	}
private:
	virtual void init_impl() {}
	virtual void close_impl() {}
	std::string name_;
};

}}

#endif
