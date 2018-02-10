/* ***********************************************************************
 * Open Web Server
 * (C) 2018 by Yiannis Bourkelis (hello@andama.org)
 *
 * This file is part of Open Web Server.
 *
 * Open Web Server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Open Web Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Open Web Server.  If not, see <http://www.gnu.org/licenses/>.
 * ***********************************************************************/

//eddieh/libevent-echo-server
//https://github.com/eddieh/libevent-echo-server/blob/master/echo-server.c

//eddieh/libuv-echo-server
//https://github.com/eddieh/libuv-echo-server/blob/master/echo-server.c

#ifndef POLL_SERVER_H
#define POLL_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream> //std::cout, memset
#include <vector>
#include <string.h> //memset
#include <QThread> //sleep

#include "message_composer.h"
#include <memory>

#ifndef WIN32
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_ntop
#include <unistd.h> //close
#include <netinet/tcp.h> //gia to TCP_NODELAY
#else
//#define NOMINMAX
#include <stdio.h>
#include "winsock2.h"
#include <ws2tcpip.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#define in_addr_t uint32_t
#pragma comment(lib, "Ws2_32.lib")
#endif

class PollServer
{
public:
    //constructors
    PollServer();

    //enums
    enum protocol {IPv4, IPv4_IPv6};

    //variables
    //std::unique_ptr<MessageComposer> msgComposer;
    MessageComposer *msgComposer;

    //static variable
    static int s_listen_sd;

    //methods

    //starts the poll server
    void start(int server_port, protocol ip_protocol);

    //ssl specific init and cleanup
    void InitializeSSL();
    void DestroySSL();
    void create_context();
    void configure_context();

    void displayLastError(std::string description);

    #ifdef WIN32
    void disableNagleAlgorithm(SOCKET socket);
    #else
    void disableNagleAlgorithm(int socket);
    #endif

    #ifdef WIN32
    void setSocketNonBlocking(SOCKET socket);
    #else
    void setSocketNonBlocking(int socket);
    #endif

    void checkClosedSessions();
private:
    //variables
    //SSL_CTX *sslctx_;
    //std::map<int, SSL*> sslmap_;
    bool use_ssl_ = false;
    struct pollfd fds[100000];

};

#endif // POLL_SERVER_H
