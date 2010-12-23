#include "log_test.hpp"
#include "logutils.hpp"

int log_test(int argc, char **argv)
{
    using namespace mhe::utils;
    boost::shared_ptr<mhe::stdlog> sl(new mhe::stdlog);
    global_log::instance().add(sl);
    boost::shared_ptr<mhe::filelog> fl(new mhe::filelog);
    fl->open("test_log.txt");
    global_log::instance().add(fl);

    global_log::instance().write("first test string");
    global_log::instance().printf("test string #%d", 2); // not safe function
    return 0;
}
