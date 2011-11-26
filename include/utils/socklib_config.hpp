#ifndef _SOCKLIB_CONFIG_HPP_
#define _SOCKLIB_CONFIG_HPP_

#include "types.hpp"

#ifdef __WIN32__
	#include <Winsock2.h>

	#define socket_t		SOCKET
	#define socket_close	closesocket
	#define addrlen_t 		int
	#define socket_error	SOCKET_ERROR
	#define invalid_socket	INVALID_SOCKET

	#define WIN_NET_START()						\
	{                               			\
		WSADATA wsaData;						\
		WSAStartup(MAKEWORD(2, 2), &wsaData);   \
	}
#endif

#ifdef __linux__
    #include <arpa/inet.h>

    #define socket_t        int
    #define socket_close    ::close
    #define addrlen_t       socklen_t
    #define socket_error    -1
    #define invalid_socket  -1
#endif


#endif
