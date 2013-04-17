#include "utils/log.hpp"
#include "utils/global_log.hpp"

namespace mhe {
namespace utils {
namespace ios {

class iOSLog : public Log
{
private:
	void write_impl(const std::string& str);
};

inline void create_standart_log()
{
	GlobalLog::instance().add(new iOSLog);
}

}}}
