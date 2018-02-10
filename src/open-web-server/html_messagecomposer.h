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

#ifndef HTML_MESSAGECOMPOSER_H
#define HTML_MESSAGECOMPOSER_H

#include "message_composer.h"
#include "http_message_dispatcher.h"
#include <memory>


class HTML_MessageComposer : public MessageComposer
{
public:
    HTML_MessageComposer();

    //varibales
    HTTP_Message_Dispatcher http_msg_dispatcher_;

    //public interface
    virtual void onNewClientConnect();
    virtual void onClientDisconnect(const int socket);
    virtual void onClientDataArrived(const int socket, const std::vector<char> &data, const int data_size, bool &pollout_events, bool &should_close_socket);
    virtual void onClientReadySend(const int socket, bool &pollout_events, bool &should_close_socket);
    virtual bool verifyMessageComplete(const std::unordered_map<int, ClientSession>::iterator msg_it, bool &pollout_events, bool &should_close_socket);
    virtual bool verify_new_MessageComplete(ClientSession &client_session, bool &pollout_events, bool &should_close_socket);
private:
    //variables
};

#endif // HTML_MESSAGECOMPOSER_H
