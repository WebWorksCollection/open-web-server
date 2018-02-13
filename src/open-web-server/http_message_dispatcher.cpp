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

//HTTP 1.1 Connections
//https://www.w3.org/Protocols/rfc2616/rfc2616-sec8.html

#include "http_message_dispatcher.h"

#include <string>
#include <iostream>
#include "cache_key.h"

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

#include "html_messagecomposer.h"
#include <QDateTime>
#include "http_response_templates.h"

#include <condition_variable>

#include "helper_functions.h"
#include <QDir>
#include <QUrl>
#include <string>
#include <sstream>
#include <iostream>
#include <QString>

HTTP_Message_Dispatcher::HTTP_Message_Dispatcher()
{
}

void HTTP_Message_Dispatcher::onClientDisconnect(const int socket)
{
    pending_responds_.erase(socket);
}

void HTTP_Message_Dispatcher::onClientReadySend(const int socket, bool &pollout_events, bool &should_close_socket){
    auto it = pending_responds_.find(socket);
    proccess_pending_client_session_queue(it, pollout_events, should_close_socket);
}

void HTTP_Message_Dispatcher::onNewMessageArrived(ClientSession &client_session, bool &pollout_events, bool &should_close_socket)
{
    qwe(std::string(client_session.request.request_header.begin(), client_session.request.request_header.end()),"");

    auto it = pending_responds_.find(client_session.socket);

    if (it == pending_responds_.end()){
        //nea kataxwrisi

        //std::queue<ClientSession> q;
        //q.push(std::move(client_session));
        //auto pair = pending_responds_.emplace(std::make_pair(client_session.socket, std::move(q)));
        //proccess_pending_client_session_queue(pair.first, pollout_events, should_close_socket);

        proccess_new_client_session(client_session, pollout_events, should_close_socket);
    } else {
        //yparxei kai alli kataxwrisi gia to idio socket
        it->second.push(std::move(client_session));
        proccess_pending_client_session_queue(it, pollout_events, should_close_socket);
    }
}

void HTTP_Message_Dispatcher::proccess_new_client_session(ClientSession &client_session, bool &pollout_events, bool &should_close_socket)
{
    for(;;){
        if (client_session.is_cached == false)
        {
            onClientRequest(client_session);

        }

#ifndef WIN32
        ssize_t res = SendData(client_session);
#else
        long res = SendData(client_session);
#endif

        if (res > 0 && client_session.send_message_has_more_bytes == false){
            //stalthikan olla ta data, opote den xreiazetai na kanoume kati allo
            should_close_socket = !client_session.request.keep_alive;
            pollout_events = false;
            break;

        } else if (res > 0 &&  client_session.send_message_has_more_bytes == true){
          //den stalthikan olla ta data,opote synexizw tin apostoli
           continue;

        } else if (res == -1 && client_session.ewouldblock_flag == true){
            //den kanw tipota allo kai perimeno
            //na ginei to socket etoimo gia send
            client_session.ewouldblock_flag = false;
            std::queue<ClientSession> q;
            q.push(std::move(client_session));
            pending_responds_.emplace(std::make_pair(client_session.socket, std::move(q)));

            pollout_events = true;
            break;
        } else {
            //allo sfalma i ekleise to connection
            pollout_events = false;
            should_close_socket = true;
            break;
        }
    }//for
}

void HTTP_Message_Dispatcher::proccess_pending_client_session_queue(std::unordered_map<int, std::queue<ClientSession>>::iterator it, bool &pollout_events, bool &should_close_socket)
{
    for(;;){
        if (it->second.front().ewouldblock_flag == false && it->second.front().is_cached == false)
        {
            onClientRequest(it->second.front());

            /*
            //socket latency test
            //it->second.front().request.parse();
            std::string resp ("HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 5"
                               "\r\n\r\n"
                               "Hello");
            it->second.front().send_message.insert(it->second.front().send_message.end(), resp.begin(), resp.end());
            */
        }

#ifndef WIN32
        ssize_t res = SendData(it->second.front());
#else
        long res = SendData(it->second.front());
#endif

        if (res > 0 && it->second.front().send_message_has_more_bytes == false){
            //stalthikan olla ta data, opote to vgazw apo to queue

            should_close_socket = !it->second.front().request.keep_alive;

            if (it->second.size() == 1){
                pending_responds_.erase(it);
                pollout_events = false;
                break;
            }

            //yprxoun kai alla responses gia apostoli
            //opote afairw mono tin trexousa pou stalthike
            it->second.pop();


        } else if (res > 0 &&  it->second.front().send_message_has_more_bytes == true){
          //den stalthikan olla ta data,opote synexizw tin apostoli
           continue;

        } else if (res == -1 && it->second.front().ewouldblock_flag == true){
            //den kanw tipota allo kai perimeno
            //na ginei to socket etoimo gia send
            it->second.front().ewouldblock_flag = false;
            pollout_events = true;
            break;
        } else {
            //allo sfalma i ekleise to connection

            if (it->second.size() == 1){
                pending_responds_.erase(it);
                pollout_events = false;
                should_close_socket = true;
                break;
            }
            it->second.pop();
        }
    }//for
}

#ifndef WIN32
ssize_t HTTP_Message_Dispatcher::SendData(ClientSession &client_session){
#else
long HTTP_Message_Dispatcher::SendData(ClientSession &client_session){
#endif
    size_t bytes_to_send = 0;
    std::vector<char> * response_data;
    int snd = 0;
    size_t total_bytes = 0;
    size_t bytesleft = 0;
    bool socket_error = false;

    if (client_session.is_cached){
        //cached
        response_data = &client_session.cache_iterator->second;
        bytes_to_send = response_data->size() - client_session.send_message_index_total;
    }else {
        //NOT cached
        response_data = &client_session.send_message;
        bytes_to_send = response_data->size() - client_session.send_message_index;
    }

    bytesleft = bytes_to_send;

    while (total_bytes < bytes_to_send){
        snd = send(client_session.socket,
                   response_data->data() + total_bytes + client_session.send_message_index,
                   bytesleft, 0);

        if (snd == -1){
            if (errno == EWOULDBLOCK || errno == EAGAIN){
                //thetw to flag se true, wste sto epomeno loop na to vgalei apo tin
                //prwti thesi tou queue kai na to valei stin teleftaia, wste na epeksergastoun
                //alla sockets an yparxoun.
                client_session.ewouldblock_flag = true;
                client_session.send_message_index += total_bytes;
                client_session.send_message_index_total += total_bytes;
                return -1;
            }

            //allo sfalma opote teleiwnw tin epeksergasia
            socket_error = true;
            client_session.ewouldblock_flag = false;
            return -1;
        }

        if (snd == 0){
            //remote peer closed the connection
            socket_error = true;
            client_session.ewouldblock_flag = false;
            return 0;
        }

        total_bytes += snd;
        bytesleft -= snd;
    }//while send loop

    client_session.send_message_index += total_bytes;
    client_session.send_message_index_total += total_bytes;

    return total_bytes;
}


void HTTP_Message_Dispatcher::onClientRequest(ClientSession &client_session){
    //************************************************************************
    //elegxw ean to arxeio einai stin cache.
    //1.Ean yparxei episttefw ton iterator. De xreiazetai na ginei kati allo
    //2.Ean den yparxei elegxw ean mporei na mpei stin cache
    //2.1.Ean mporei na mpei stin cache epistrefw ton iterator. De xreiazetai na ginei kati allo
    //2.2.Ean den mporei na mpei stin cache (p.x. megalo megethos) synexizw xwrizontas to arxeio ana mimata an xreiazetai
    if (client_session.cache_cheked == false){
        client_session.cache_cheked = true;

        //parse to request. lamvanw to path pou zitithike klp...
        client_session.request.parse();

        if (client_session.request.has_range_header == false){//pros to paron ta requests me range DEN apothikevontai stin cache
            //elegxw ean to arxeio pou zitithike vrisketai stin cache
            client_session.cache_iterator = shared_cache_.cache_.find(client_session.request.get_hostname_and_requested_path());
            bool cached = (client_session.cache_iterator != shared_cache_.cache_.end());

            if (cached) {
                //ean vrisketai stin cache tote apla thetw to is_cached se true wste na kserei o
                //sender na to xrisimopoiisei apo tin cache kanontas xrisi tou map iterator
                client_session.is_cached = true;
                client_session.request.requested_file_size = client_session.cache_iterator->second.size(); //TODO: na dw an xrisimopoiw kapou to requested_file_size se periptwsi cached dedomenwn
                return;
            } else {
                //den yparxei stin cache opote elegw mipws mporei na mpei
                if (check_add_to_cache(client_session) == true) {
                    return;
                }
            }//cached
        } else {//if has range header
            //to kalw gia na parei to absolute path timi giati otan yparxei range header
            //den ypologizetai to absolute path
            server_config_->is_valid_requested_hostname(client_session.request);
        }

    }//elegxos ean einai cached to resource.
    //************************************************************************

    //1. to resource pou zitithike den mporei na mpei stin cache,
    //opote elegxw ti xreiazetai na kanw parakatw,
    //analogws ean zitithike arxeio i folder.
    //2. h zitithike range tou resource.
    load_and_proccess_file(client_session);
}

void HTTP_Message_Dispatcher::load_and_proccess_file(ClientSession &client_session){
    //ean to arxeio DEN vrisketai stin cache, to fortonw apo to filesystem
    //kai to topothetw stin cache
    std::vector<char> response_body_vect;
    bool file_found = loadFileToVector(client_session, response_body_vect);

    if (file_found){
        //lifthikan ta dedomena tou arxeio, opote proxoro se
        //dimiourgia tou HTTP reply me ta data tou arxeiou
        proccessFile(client_session, response_body_vect, false);
    } else {
        //den prokeitai gia arxeio, opote elegxw mipws afora
        //directory browsing
        if (!proccessDirectory(client_session)){
            //ean den afora oute directory listing tote 404
            proccess404(client_session);
        }
    }
}

bool HTTP_Message_Dispatcher::check_add_to_cache(ClientSession &client_session){
    //elegxw ean to hostname pou zitithike anikei se virtual server.
    //1. Ean anikei, thetei sto client_session.request.absolute_hostname_and_requested_path_
    //to absolute path gia to uri pou zitithike.
    //2. Ean den anikei tote psaxei gia default vistual host klp
    if (server_config_->is_valid_requested_hostname(client_session.request) == false){
        return false;
    }

    if(is_malicious_path(client_session.request.absolute_hostname_and_requested_path_)) return false;

    //prospathw na anoiksw to arxeio pou zitithike
    QFile file_io(client_session.request.absolute_hostname_and_requested_path_);
    if (file_io.open(QFileDevice::ReadOnly) == false) {
        //Ean den vrethike to arxeio pou zitithike, elegxw ta indexes
        if (server_config_->index_exists(client_session, file_io) == false){
            //ean to arxeio den yparxei sto filesystem, epistrefw false, wste na ginei
            //elegxos sti synexeia, ean prokeitai gia directory, i na epistrafei 404 sfalma
            return false;
        }
    }

    //elegxw ean to megethos tou arxeiou epitrepetai na mpei stin cache
    if (file_io.size() <= shared_cache_.max_file_size){
        //elegxw ean to neo arxeio epitrepetai na mpei stin cache
        if (shared_cache_.cache_current_size + file_io.size() <= shared_cache_.cache_max_size){
            //ok mporei na mpei stin cache

            long vres = file_io.size();
            std::vector<char> response_body_vect;
            response_body_vect.resize(vres);
            file_io.read(response_body_vect.data(), vres);
            client_session.request.requested_file_size = vres;

            proccessFile(client_session, response_body_vect, true);

            //HTTP_Message_Dispatcher::cache_[client_session.request.request_path] = std::move(response_body_vect);
            //client_session.cache_iterator = HTTP_Message_Dispatcher::cache_.find(client_session.request.request_path);
            shared_cache_.cache_current_size += response_body_vect.size();
            client_session.cache_iterator = shared_cache_.cache_.emplace(
                        std::make_pair(CacheKey(client_session.request.get_hostname_and_requested_path(),client_session.request.absolute_hostname_and_requested_path_), std::move(response_body_vect))).first;
            client_session.is_cached = true;

            //afou topothetithike to arxeio stin cache, to kataxwrw kai sto filesystemwatcher
            shared_cache_.file_system_watcher.addPath(client_session.request.absolute_hostname_and_requested_path_);
            return true;
        }
    }
    return false; //to arxeio den epitrepetai na mpei stin cache
}


//epistrefei false ean den vrethike to arxeio, alliws true
bool HTTP_Message_Dispatcher::loadFileToVector(ClientSession &client_session, std::vector<char> &response_body_vect){
    if(is_malicious_path(client_session.request.absolute_hostname_and_requested_path_)) return false;

    QFile file_io(client_session.request.absolute_hostname_and_requested_path_);

    if (file_io.open(QFileDevice::ReadOnly) == false) return false;

    //TODO: na elegxw oti me to request.range_from_byte eimaste ok se periptwsi pou ginei seek pera apo to megethos tou arxeiou
    file_io.seek(client_session.request.range_from_byte + client_session.send_message_index_total - client_session.send_message_header_size);
    client_session.request.requested_file_size = file_io.size();

    qint64 remaining_file_size = 0;
    if (client_session.request.has_range_header == false){
        remaining_file_size = client_session.request.requested_file_size - client_session.send_message_index_total + client_session.send_message_header_size;
    }else {
        remaining_file_size = client_session.request.range_until_byte - client_session.request.range_from_byte - client_session.send_message_index_total + client_session.send_message_header_size + 1;
    }

    if (remaining_file_size > FILE_CHUNK){
        //to arxeio den xoraei olokliro sto send_message buffer opote diavazw mono ena tmima tou
        //me megethos oso oristike apo to FILE_CHUNK
        response_body_vect.resize(FILE_CHUNK);
        file_io.read(response_body_vect.data(), FILE_CHUNK);
        client_session.send_message_has_more_bytes = true;

    } else if (remaining_file_size <= FILE_CHUNK) {
        //edw erxetai ean to arxeio xwraei sto send_message buffer, i ean einai to teleftaio tmima tou arxeiou
        //bytes.resize(file_io.size() - client_session.send_message_index + client_session.send_message_header_size);
        //file_io.read(bytes.data(), file_io.size() - client_session.send_message_index + client_session.send_message_header_size);
        response_body_vect.resize(remaining_file_size);
        file_io.read(response_body_vect.data(), remaining_file_size);
        client_session.send_message_has_more_bytes = false;
    }

    return true;
}

bool HTTP_Message_Dispatcher::is_malicious_path(QString path)
{
    return path.contains("..");
}

void HTTP_Message_Dispatcher::proccessFile(ClientSession &client_session, std::vector<char> &response_body_vect, bool merge_only){
    std::vector<char> resp_vector;

    //ean eimaste stin prwti apostoli tou arxeiou
    if (client_session.send_message_index_total == 0){

        //lamvanw to mime tou arxeiou gia na to
        //steilw sto response
        mime_type_ = mime_db_.mimeTypeForFile(client_session.request.request_uri);//ok einai grigori i function

        //topothetw ta header values sto response
        std::string resp_string;
        if (client_session.request.has_range_header == false){
            //to length tou body se QString
            std::stringstream ssize_;
            ssize_ << client_session.request.requested_file_size;
            std::string sService = ssize_.str();
            QString length_(QString::fromStdString(sService));

            resp_string = HTTP_Response_Templates::_200_OK.arg(mime_type_.name(), length_).toStdString();
        }else {
            std::stringstream range_from_byte;
            range_from_byte << client_session.request.range_from_byte;

            std::stringstream range_until_byte;
            range_until_byte << client_session.request.range_until_byte;

            std::stringstream requested_file_size;
            requested_file_size << client_session.request.requested_file_size;

            unsigned long long length = client_session.request.range_until_byte - client_session.request.range_from_byte + 1;
            std::stringstream length_stream;
            length_stream << length;

           resp_string = HTTP_Response_Templates::_206_PARTIAL_CONTENT_RESPONSE_HEADER.arg

                   (mime_type_.name(),
                   QString::fromStdString(length_stream.str()),
                    QString::fromStdString(range_from_byte.str()),
                    QString::fromStdString(range_until_byte.str()),
                    QString::fromStdString(requested_file_size.str()))
                   .toStdString();
        }

        client_session.send_message_header_size = resp_string.size();

        //prosthetw to header
        resp_vector.insert(resp_vector.end(), resp_string.begin(), resp_string.end());
    }

    //prostheto sto telos tou header vector (mporei na min yparxoun header data)
    //ta dedomena tou body
    client_session.send_message_index = 0;
    resp_vector.insert(resp_vector.end(), response_body_vect.begin(), response_body_vect.end());

    if(merge_only) {
        response_body_vect = std::move(resp_vector);
    } else {
        client_session.send_message = std::move(resp_vector);
    }
}

void HTTP_Message_Dispatcher::proccess404(ClientSession &client_session){
    //to length tou body se QString
    std::stringstream ssize_;
    ssize_ << HTTP_Response_Templates::_404_NOT_FOUND_.size();
    std::string sService = ssize_.str();
    QString length_(QString::fromStdString(sService));

    QString text_html = "text/html";
    QString header_404 = HTTP_Response_Templates::_404_.arg(text_html, length_);
    client_session.send_message_header_size = header_404.size();
    client_session.send_message = HTTP_Response_Templates::merge(header_404, HTTP_Response_Templates::_404_NOT_FOUND_);
    client_session.request.requested_file_size = HTTP_Response_Templates::_404_NOT_FOUND_.size();
}

//epeskergasia aitimatos gia provoli periexomenwn enos directory.
//Den ginetai pote cache to response.
//Epistrefei true ean yparxei to directory alliws false
bool HTTP_Message_Dispatcher::proccessDirectory(ClientSession &client_session){
    QDir directory;
    QFileInfoList list;
    std::wostringstream os;

    if (directory.setCurrent(client_session.request.absolute_hostname_and_requested_path_) == false) return false;

    //directory
    directory.setFilter(QDir::NoDot | QDir::AllEntries);
    directory.setSorting(QDir::DirsFirst);
    list = directory.entryInfoList();
    std::wstring url_encoded;
    for (auto file:list){
        QString slash("/");
        if(file.isDir()){
            url_encoded = file.fileName().replace(" ", "%20").toStdWString() + slash.toStdWString();
            //einai directory
            os << "<br /><a href=""" << url_encoded << """>"
             << QString("<DIR> ").toHtmlEscaped().toStdWString()
            << file.fileName().toHtmlEscaped().toStdWString() << "</a>";
        }else {
            url_encoded = file.fileName().replace(" ", "%20").toStdWString();
            //einai arxeio
            os << "<br /><a href=""" << (client_session.request.request_uri.endsWith(slash) ?
                                             client_session.request.request_uri.toStdWString() :
                                             client_session.request.request_uri.toStdWString() +  slash.toStdWString())
                                        + url_encoded << """>"
            << file.fileName().toHtmlEscaped().toStdWString() << "</a>";
        }
    }//for loop

    std::wstring response_body = HTTP_Response_Templates::_DIRECTORY_LISTING_.arg(client_session.request.get_hostname_and_requested_path(),
                                                                     QString::fromStdWString(os.str())).toStdWString();

    //to length tou body se QString
    std::stringstream ssize_;
    ssize_ << response_body.size();
    std::string sService = ssize_.str();
    QString length_(QString::fromStdString(sService));

    QString mime_type("text/html");

    //topothetw ta header values sto response
    std::string resp_header_string = HTTP_Response_Templates::_200_OK.arg(mime_type, length_).toStdString();

    //prosthetw sto header to body
    client_session.request.requested_file_size = response_body.size();
    client_session.send_message_header_size = resp_header_string.size();
    std::vector<char> resp_vector(std::make_move_iterator(resp_header_string.begin()), std::make_move_iterator(resp_header_string.end()));
    resp_vector.insert(resp_vector.end(), std::make_move_iterator(response_body.begin()), std::make_move_iterator(response_body.end()));

    client_session.send_message_index = 0;
    client_session.is_cached = false;
    client_session.send_message = std::move(resp_vector);

    return true;
}
