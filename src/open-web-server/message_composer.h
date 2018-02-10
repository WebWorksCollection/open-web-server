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

#ifndef MESSAGE_COMPOSER_H
#define MESSAGE_COMPOSER_H

#include <unordered_map>
#include <vector>
#include "clientsession.h"

class MessageComposer
{
public:
    //constructor
    MessageComposer();

    //destructor
    virtual ~MessageComposer();

    //public interface
    virtual void onNewClientConnect();
    virtual void onClientDisconnect(const int socket);
    virtual void onClientDataArrived(const int socket, const std::vector<char> &data, int data_size, bool &pollout_events, bool &should_close_socket);
    virtual void onClientReadySend(const int socket, bool &pollout_events, bool &should_close_socket);

protected:
    std::unordered_map<int, ClientSession> messages_;
};

#endif // MESSAGE_COMPOSER_H
