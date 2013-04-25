#ifndef __IOS_APPLICATION_HPP__
#define __IOS_APPLICATION_HPP__

#include "app/application.hpp"
#include "ios_log.hpp"

@class iOSApplicationImpl;

namespace mhe {
namespace app {
namespace ios {

class iOSApplication : public Application
{
public:
	iOSApplication();
private:
	bool mhe_app_init(const ApplicationConfig& config);
    void mhe_app_deinit();
	std::string application_base_path() const;

    iOSApplicationImpl* impl_;
};

}}}

#endif
