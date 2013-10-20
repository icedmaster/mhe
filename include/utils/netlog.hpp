#ifndef _NETLOG_HPP_
#define _NETLOG_HPP_

#include "socklib.hpp"
#include "log.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	const uint16_t default_netlog_port = 5223;

	class netlog : public log
	{
		private:
			boost::shared_ptr<sock::socket> sock;

			void close_impl()
			{
				sock->close();
			}

			void write_impl(const std::string& s)
			{
				sock->write(s.c_str(), s.length());
			}

			void write_impl(const std::wstring&/* ws*/)
        	{}
		public:
			netlog() :
				log()
			{}

			netlog(boost::shared_ptr<sock::socket> ns) :
				log(),
				sock(ns)
			{}

			~netlog()
			{}

			bool open(sock::socket_type type = sock::socket_udp_type,
					  uint16_t port = default_netlog_port)
			{
            	using namespace sock;
				if (is_open()) return false;
				if (type == socket_udp_type)
				{
					boost::shared_ptr<sock::socket> t(new udp_socket);
					sock = t;
				}
				else if (type == socket_tcp_type)
				{
					boost::shared_ptr<sock::socket> t(new tcp_socket);
                	sock = t;
				}
				else return false;

				bool res = sock->open(port, 0);
				if (!res) return false;

				if (type == socket_tcp_type)
				{
					res = sock->connect();
					if (!res) return false;
				}

				set_open();
				return true;
			}
	};
};

#endif
