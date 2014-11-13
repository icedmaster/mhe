#ifndef __SOCKLIB_CONFIG_HPP__
#define __SOCKLIB_CONFIG_HPP__

#include "core/types.hpp"
#include "core/config.hpp"

#ifdef MHE_WIN
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

#ifdef MHE_LINUX
    #include <arpa/inet.h>
    #include <unistd.h>

    #define socket_t        int
    #define socket_close    ::close
    #define addrlen_t       socklen_t
    #define socket_error    -1
    #define invalid_socket  -1
#endif


#endif
