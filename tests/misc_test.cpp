#include "misc_test.hpp"
#include "camera.hpp"
#include "argparser.hpp"
#include "logutils.hpp"

int misc_test(int argc, char **argv)
{
    mhe::arg_parser ap(argc, argv);

    mhe::mixlog l;
    boost::shared_ptr<mhe::stdlog> sl(new mhe::stdlog);
    boost::shared_ptr<mhe::filelog> fl(new mhe::filelog);
    l.add(sl);
    l.add(fl);

    l.write("Misc testing begin!!");
    mhe::Camera camera1(mhe::v3d(0.5, 0.5, -2), mhe::v3d(0, 0, 0));

    l.write("Misc test completed");
    return 0;
}
