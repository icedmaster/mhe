#ifndef _SOCKLIB_HPP_
#define _SOCKLIB_HPP_

#include "socklib_config.hpp"
#include <string>
#include <cstring>  // for memset()

namespace sock
{

	enum socket_type
	{
		socket_unknown_type,
		socket_raw_type,
		socket_tcp_type,
		socket_udp_type
	};

	enum socket_state
	{
		socket_closed_state,
		socket_opened_state,
		socket_binded_state,
		socket_connected_state
	};

	enum socket_mode
	{
		socket_unknown_mode,
		socket_server_mode,
		socket_client_mode
	};

	struct exception
	{
		std::string txt;

		exception(const std::string& s) :
			txt(s)
		{}
	};

	// Abstract socket interface
	class socket
	{
		public:
			struct socket_addr
			{
				cmn::uint16_t port;
				cmn::uint32_t addr;
			};
		private:
			socket_t s;
			socket_state state_;
			socket_type type_;
			socket_addr saddr;	// self address
			socket_addr raddr;	// remote address
			socket_mode mode_;

			void close_p()
			{
				socket_close(s);
				state_ = socket_closed_state;
			}

			bool open_p()
			{
				bool is_open_ = open_impl();
				if (is_open_)
					state_ = socket_opened_state;
				return is_open_;
			}

			bool bind_p()
			{
				bool is_binded_ = bind_impl();
				if (is_binded_)
					state_ = socket_binded_state;
				return is_binded_;
			}

			bool connect_p(socket_mode m, int timeout)
			{
				mode_ = m;
				bool is_connected_ = connect_impl(timeout);
				if (is_connected_)
					state_ = socket_connected_state;
				return is_connected_;
			}

			void disconnect_p()
			{
				close_p();
				state_ = socket_closed_state;
			}

			int write_p(const char *buf, int size, int flags)
			{
				return write_impl(buf, size, flags);
			}

			int read_p(char *buf, int size, int flags, int timeout)
			{
				return read_impl(buf, size, flags, timeout);
			}

		protected:
			static const addrlen_t c_addr_len = sizeof(sockaddr_in);

			void set_socket(socket_t ns)
			{
				s = ns;
			}

			socket_t get_socket() const
			{
				return s;
			}

			void set_type(socket_type type)
			{
				type_ = type;
			}

			const socket_addr& selfaddr() const
			{
				return saddr;
			}

			const socket_addr& remoteaddr() const
			{
				return raddr;
			}

			void set_remoteaddr(cmn::uint16_t port, cmn::uint32_t addr)
			{
				raddr.port = port;
            	raddr.addr = addr;
            }

			virtual bool open_impl() = 0;

			virtual bool bind_impl()
			{
				sockaddr_in addr;
				memset((char*)&addr, 0, c_addr_len);
				addr.sin_family = AF_INET;
				addr.sin_port   = htons(saddr.port);
				addr.sin_addr.s_addr   = saddr.addr;
				if (::bind(s, (sockaddr*)&addr, c_addr_len) == socket_error)
					return false;
				return true;
			}

			virtual bool connect_impl(int/* timeout*/)
			{
				return false;
			}

			virtual int write_impl(const char *buf, int size, int flags) = 0;
			virtual int read_impl(char *buf, int size, int flags, int timeout) = 0;
		public:
			socket() :
				s(0),
				state_(socket_closed_state),
				type_(socket_unknown_type)
			{
				saddr.port = 0;
				saddr.addr = 0;
			}

			~socket()
			{
				if (state_ > socket_closed_state)
					close_p();
			}

			socket_type type() const
			{
				return type_;
			}

			// state attributes
			bool is_open() const
			{
				return (state_ > socket_closed_state) ? true : false;
			}

			bool is_binded() const
			{
				return (state_ > socket_opened_state) ? true : false;
			}

			bool is_connected() const
			{
				return (state_ > socket_connected_state) ? true : false;
			}

			socket_mode mode() const
			{
				return mode_;
			}

			bool open()
			{
				return open_p();
			}

			bool open(cmn::uint16_t port, cmn::uint32_t addr = 0)
			{
				if (!open_p())
					return false;
				return bind(port, addr);
			}

			bool open(uint16_t port, const std::string& addr)
			{
				return open(port, inet_addr(addr.c_str()));
			}

			bool bind(cmn::uint16_t port, cmn::uint32_t addr = 0)
			{
				saddr.port = port;
				saddr.addr = addr;
				return bind_p();
			}

			bool bind(cmn::uint16_t port, const std::string& addr)
			{
				return bind(port, inet_addr(addr.c_str()));
			}

			void close()
			{
				close_p();
			}

			bool connect(int timeout = 0)		// as server
			{
				return connect_p(socket_server_mode, timeout);
			}

			bool connect(cmn::uint16_t port, cmn::uint32_t addr, int timeout = 0)
			{
				raddr.port = port;
				raddr.addr = addr;
				return connect_p(socket_client_mode, timeout);
			}

			void disconnect()
			{
				disconnect_p();
			}

			int write(const char *buf, int size, int flags = 0)
			{
				return write_p(buf, size, flags);
			}

			int read(char *buf, int size, int flags = 0, int timeout = 0)
			{
				return read_p(buf, size, flags, timeout);
			}
	};

	class udp_socket : public socket
	{
		private:
			bool open_impl()
			{
				socket_t ss = ::socket(AF_INET, SOCK_DGRAM, 0);
				set_socket(ss);
				if (ss != invalid_socket)
					return true;
				return false;
			}

			int write_impl(const char *buf, int size, int flags)
			{
				sockaddr_in outaddr;
				memset((char*)&outaddr, 0, c_addr_len);
				outaddr.sin_family = AF_INET;
				outaddr.sin_port   = htons(remoteaddr().port);
				outaddr.sin_addr.s_addr = remoteaddr().addr;
				return sendto(get_socket(), buf, size, flags, (sockaddr*)&outaddr, sizeof(outaddr));
			}

			int read_impl(char *buf, int size, int flags, int/* timeout*/)
			{
				sockaddr_in inaddr;
				memset((char*)&inaddr, 0, c_addr_len);
				addrlen_t len = c_addr_len;
				return recvfrom(get_socket(), buf, size, flags, (sockaddr*)&inaddr, &len);
			}

			int recv_p(cmn::uint16_t& port, cmn::uint32_t& addr, char *buf, int size, int flags, int/* timeout*/)
			{
				sockaddr_in inaddr;
				memset((char*)&inaddr, 0, c_addr_len);
				addrlen_t len = c_addr_len;
				int rlen = recvfrom(get_socket(), buf, size, flags, (sockaddr*)&inaddr, &len);
				port = htonl(inaddr.sin_port);
				addr = inaddr.sin_addr.s_addr;
				return rlen;
			}
		public:
			udp_socket() :
				socket()
			{
				set_type(socket_udp_type);
			}

			~udp_socket()
			{}

			int send(cmn::uint16_t port, uint32_t addr, const char *buf, int size, int flags = 0)
			{
				// do not need to check result - UDP desu
				connect(port, addr);
				return write(buf, size, flags);
			}

			int recv(cmn::uint16_t& port, cmn::uint32_t& addr, char *buf, int size, int flags = 0, int timeout = 0)
			{
				return recv_p(port, addr, buf, size, flags, timeout);
			}
	};

	class tcp_socket : public socket
	{
		private:
        	socket_t cl_s;		// client socket
			bool open_impl()
			{
				socket_t ss = ::socket(AF_INET, SOCK_STREAM, 0);
				set_socket(ss);
				if (ss != invalid_socket)
					return true;
				return false;
			}

			bool connect_impl(int timeout)
			{
				if (mode() == socket_server_mode)
					return connect_as_server(timeout);
				else if (mode() == socket_client_mode)
					return connect_as_client(timeout);
            	return false;
			}

			bool connect_as_server(int/* timeout*/)
			{
				int err = ::listen(get_socket(), 1);
				if (err == socket_error) return false;
				sockaddr_in addr;
				memset((char*)&addr, 0, c_addr_len);
            	addrlen_t len = c_addr_len;
				cl_s = ::accept(get_socket(), (sockaddr*)&addr, &len);
				if(cl_s == invalid_socket) return false;
				set_remoteaddr(htonl(addr.sin_port), addr.sin_addr.s_addr);
            	return true;
			}

			bool connect_as_client(int/* timeout*/)
        	{
				sockaddr_in addr;
				memset((char*)&addr, 0, c_addr_len);
				addr.sin_family = AF_INET;
				addr.sin_port   = htons(remoteaddr().port);
            	addr.sin_addr.s_addr = remoteaddr().addr;
				cl_s = ::connect(get_socket(), (sockaddr*)&addr, c_addr_len);
				if (cl_s == invalid_socket)
					return false;
            	return true;
			}

			int write_impl(const char* buf, int size, int flags)
        	{
				int slen = 0;
				while (slen != size)
            	{
					int len = send(cl_s, buf + slen, size - slen, flags);
					if (len <= 0)
						break;
					slen += len;
				}
				return slen;
			}

			int read_impl(char* buf, int size, int flags, int/* timeout*/)
			{
				int slen = 0;
				while (slen != size)
				{
					int len = recv(cl_s, buf + slen, size - slen, flags);
					if (len <= 0)
						break;
                	slen += len;
				}
            	return slen;
            }

		public:
			tcp_socket() :
				socket(),
				cl_s(invalid_socket)
			{
            	set_type(socket_tcp_type);
			}

			~tcp_socket()
			{
				if (is_connected() > socket_binded_state)
                	socket_close(cl_s);
			}
    };

};	// namespace

#endif
