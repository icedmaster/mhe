#include "utils/logutils.hpp"

namespace mhe {
namespace utils {

    bool createStandartLog()
    {
        boost::shared_ptr<stdlog> l1(new stdlog);
        boost::shared_ptr<filelog> l2(new filelog);
        //boost::shared_ptr<netlog> l3(new netlog);

        global_log::instance().add(l1);

        if (l2->open("log.log"))
            global_log::instance().add(l2);
        //if (l3->open())
        //    global_log::instance().add(l3);

        return true;
    }

}
}
