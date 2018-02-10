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

#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include <vector>
#include <unordered_map>
#include <http_header_parser.h>
#include "qstring_hash_specialization.h"

class ClientSession
{
public:
    ClientSession();
    ClientSession(const int __socket, const std::vector<char> &data, const int data_size);

    int socket;//to socket pou client sto opoio ginetai i apostoli dedomenwn
    bool ewouldblock_flag = false;//gia megala mynimata, ean den ginei apostoli me ena send, thetei afto to flag se true
    std::vector<char> send_message;//periexei ta dedomena pros apostoli
    bool send_message_has_more_bytes = false;//ta megala arxeia den fortwnontai oloklira sto send_message alla tmimatika. se afti tin periptwsi thetei to flag se true
    long send_message_index = 0; //posa bytes exoun synolika stalei apo to trexon send_message. perilamvanei to megethos tou HTTP response header
    long send_message_index_total = 0; //posa bytes exoun synolika stalei apo to arxeio. perilamvanei to megethos tou HTTP response header
    long send_message_header_size = 0;//to megethos tou HTTP response header. Afairontas to apo to send_message_index exw ta synolika bytes pou stalthikan
    HTTP_Header_parser request;//to request opws stalthike apo ton client



    //cache specific
    std::unordered_map<QString, std::vector<char>>::iterator cache_iterator;
    bool is_cached = false;//flag pou otan einai true simainei oti to resource tha apostalei apo tin cache.
    bool cache_cheked = false;//flag pou simatodotee ean exei ginei elegxos tou resource gia tin parousia tou stin cache

};

#endif // CLIENTSESSION_H
