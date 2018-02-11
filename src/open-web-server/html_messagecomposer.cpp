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

#include "html_messagecomposer.h"
#include <iostream>
#include <string>

HTML_MessageComposer::HTML_MessageComposer()
{

}

void HTML_MessageComposer::onNewClientConnect()
{

}

void HTML_MessageComposer::onClientDisconnect(const int socket)
{
    http_msg_dispatcher_.onClientDisconnect(socket);
}

void HTML_MessageComposer::onClientReadySend(const int socket, bool &pollout_events, bool &should_close_socket)
{
    http_msg_dispatcher_.onClientReadySend(socket, pollout_events, should_close_socket);
}

//TODO: thelei diorthwsi i methodos synthesis twn request giati den einai swsti se periptwsi pou o
//client ypostirizei HTTP pipelining, tha stelnei pollapla requests kai endexomenws na kataleiksoume
//na yparxoun perissotera apo ena request sto stored_client.
//
//TODO: gia logous asfaleias na periorizw to megethos tou request pou mporei na steilei enas client
//p.x. se 8K, kai se peripotwsi ypervasis na epistrefw 413 – Request Entity Too Large Error  kai na kleinw to connection
void HTML_MessageComposer::onClientDataArrived(const int socket, const std::vector<char> &data, const int data_size, bool &pollout_events, bool &should_close_socket)
{
    /*
    //socket latency test
    ClientSession tmpcsession;
    tmpcsession.socket = socket;
    http_msg_dispatcher_.onNewMessageArrived(tmpcsession, pollout_events, should_close_socket);
    return;
    */

    auto stored_client = messages_.find(socket);

    if (stored_client == messages_.end()){
        //den yparxoun data gia afto to socket
        //opote ta kataxwrw olla
        ClientSession tmpcsession;
        tmpcsession.socket = socket;
        std::move(data.begin(), data.begin() + data_size, std::back_inserter(tmpcsession.request.request_header));
        //ClientSession tmpcsession(socket, data, data_size);

        //elegxo ean to mynima exe symplirwthei
        if (verify_new_MessageComplete(tmpcsession, pollout_events, should_close_socket) == false){
            messages_.emplace(socket, std::move(tmpcsession));
        }
    } else {
        //proyparxoun dedomena, opote ta kanw merge
        //TODO: na to kanw me std::move
        stored_client->second.request.request_header.insert(stored_client->second.request.request_header.end(),
                                                  std::make_move_iterator(data.begin()),
                                                  std::make_move_iterator(data.end()));
        //elegxo ean to mynima exe symplirwthei
        verifyMessageComplete(stored_client, pollout_events, should_close_socket);
    }
}

bool HTML_MessageComposer::verify_new_MessageComplete(ClientSession &client_session, bool &pollout_events, bool &should_close_socket)
{
    //ean exei lifthei olokliro to request, pou simainei oti teleiwnei me \r\n\r\n
    //epistrefw true, alliws epistrefw false kai perimeno to epomeno minima gia na to enosw.
    if (client_session.request.request_header.size() > 3){
            std::vector<char>::const_iterator tmp = client_session.request.request_header.end();
            if ( (*(--tmp) == 10) && // \n
                 (*(--tmp) == 13) && // \r
                 (*(--tmp) == 10) && // \n
                 (*(--tmp) == 13) ){ // \r
            http_msg_dispatcher_.onNewMessageArrived(client_session, pollout_events, should_close_socket);
            return true;
        }//if
    }
    return false;
}

bool HTML_MessageComposer::verifyMessageComplete(const std::unordered_map<int, ClientSession>::iterator msg_it, bool &pollout_events, bool &should_close_socket)
{
    //ean exei lifthei olokliro to request, pou simainei oti teleiwnei me \r\n\r\n
    //epistrefw true, alliws epistrefw false kai perimeno to epomeno minima gia na to enosw.
    if (msg_it->second.request.request_header.size() > 3){
            std::vector<char>::iterator tmp = msg_it->second.request.request_header.end();
            if ( (*(--tmp) == 10) && // \n
                 (*(--tmp) == 13) && // \r
                 (*(--tmp) == 10) && // \n
                 (*(--tmp) == 13) ){ // \r
            http_msg_dispatcher_.onNewMessageArrived(msg_it->second, pollout_events, should_close_socket);
            messages_.erase(msg_it);           
            return true;
        }//if
    }
    return false;
}
