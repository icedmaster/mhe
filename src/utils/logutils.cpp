#include "logutils.hpp"

using namespace mhe;

boost::shared_ptr<log> mhe::utils::createStandartLog()
{
	boost::shared_ptr<mixlog> l(new mixlog);
	boost::shared_ptr<stdlog> l1(new stdlog);
	boost::shared_ptr<filelog> l2(new filelog);
	boost::shared_ptr<netlog> l3(new netlog);
	
	if ( !l2->open("log.log") || !l3->open() )
		return l;
	l->add(l1);
	l->add(l2);
	l->add(l3);
	
	return l;
}


void mhe::utils::log_write(const std::string& s)
{
}
