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

#ifndef HTTP_MESSAGE_DISPATCHER_H
#define HTTP_MESSAGE_DISPATCHER_H

#include "clientsession.h"

#include <queue>
#include "clientsession.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <QString>
#include <QMimeDatabase>
#include <QMimeType>
#include <atomic>

#include "shared_cache.h"
#include "server_config.h"

class HTTP_Message_Dispatcher
{
public:
    HTTP_Message_Dispatcher();

    //variables
    ServerConfig *server_config_;
    SharedCache shared_cache_;

    //methods

    void dispatchMessageQueue();
    void onNewMessageArrived(ClientSession &client_session, bool &pollout_events, bool &should_close_socket);
    void onClientReadySend(const int socket, bool &pollout_events, bool &should_close_socket);
    void onClientDisconnect(const int socket);
private:
    //variables
    std::unordered_map<int, std::queue<ClientSession>> pending_responds_;

    //xwrizei to arxeio pou zitithike se tmimata isa me ta bytes pou orizontai.
    //Afora arxeia pou den mporoun na mpoun stin cache
    const int FILE_CHUNK = 32768; //8192=8.1mb/s, 16384/32768=8.5mb/s 65536 131072 262144 524288 >1048576 2097152 4194304

    const QMimeDatabase mime_db_;
    QMimeType mime_type_;

    //methods
#ifdef WIN32
    long SendData(ClientSession &client_session);
#else
    ssize_t SendData(ClientSession &client_session);
#endif

    void onClientRequest(ClientSession &client_session);
    bool loadFileToVector(ClientSession &client_session, std::vector<char> &response_body_vect);
    void proccessFile(ClientSession &client_session, std::vector<char> &response_body_vect, bool merge_only);
    bool proccessDirectory(ClientSession &client_session);
    void proccess404(ClientSession &client_session);
    void load_and_proccess_file(ClientSession &client_session);
    bool check_add_to_cache(ClientSession &client_session);
    bool is_malicious_path(QString path);

    void proccess_pending_client_session_queue(std::unordered_map<int, std::queue<ClientSession>>::iterator it, bool &pollout_events, bool &should_close_socket);
    void proccess_new_client_session(ClientSession &client_session, bool &pollout_events, bool &should_close_socket);
};

#endif // HTTP_MESSAGE_DISPATCHER_H
