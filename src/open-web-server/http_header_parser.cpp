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

#include "http_header_parser.h"
#include <iostream>
#include <sstream>

HTTP_Header_parser::HTTP_Header_parser()
{
    //TODO: isws xreiazetai veltistopoiisi to megethos twn arxikwn bytes pou tha ginoun reserve
    request_header.reserve(512);
}

HTTP_Header_parser::HTTP_Header_parser(const std::vector<char> &__request_header) :
    request_header(std::move(__request_header))
{
}

int HTTP_Header_parser::parse()
{

    //if (is_dirty){

        //metatrepw to client request se string gia na to analysw
        const std::string request(request_header.begin(), request_header.end());

        //headers = split(request, '\n');


        //******************************************
        //lamvanw to directory pou zitithike
        //GET dir /r/n
        int get_start = request.find("G");
        int get_end = request.find(" ", get_start + 4);
        std::string url(request.begin() + 4, request.begin() + get_end);
        int get_end_idx = get_end + 9;
        request_uri = std::move(QString::fromStdString(url).replace("%20", " "));
        //url = "" + url + "";
        //******************************************

        //******************************************
        // lamvanw to hostname
        size_t hostname_idx = request.find("Host: ", get_end_idx + 2);
        if (hostname_idx != std::string::npos){
            size_t hostname_idx_end = request.find("\r", hostname_idx + 6);
            if (hostname_idx_end != std::string::npos){
                //std::string hostname_str = request.substr(hostname_idx + 6, hostname_idx_end - hostname_idx - 6);
                std::string hostname_str(request.begin() + hostname_idx + 6, request.begin() + hostname_idx_end);
                hostname = QString::fromStdString(hostname_str);
            }
        }
        //******************************************

        //******************************************
        //elegxos ean einai HTTP/1.0
        //size_t http1_0 = request.find("HTTP/1.0", rv - 8);
        //if (http1_0 != std::string::npos){
        //    keep_alive = false;
        //}
        //******************************************
        keep_alive = (request.find("Connection: close") == std::string::npos);


        //******************************************
        //lamvanw tyxwn byte range
         size_t range_pos = request.find("\nRange: bytes=", get_end_idx + 2);
         if (range_pos != std::string::npos){
             //vrethike range
             size_t range_dash = request.find("-", range_pos + 14);
             if (range_dash != std::string::npos){//vrethike i pavla px 0-1
                size_t range_end = request.find("\r", range_dash + 2);
                if (range_end != std::string::npos){
                 std::string from(request.begin() + range_pos + 14,
                                  request.begin() + range_dash);

                 std::string until(request.begin() + range_dash + 1,
                                   request.begin() + range_end);
                 range_from_byte = std::stoull(from);
                 range_until_byte = std::stoull(until);
                 //char *end;
                 //range_until_byte = std::strtoull(until.c_str(), &end, 10);
                 has_range_header = true;

                 //std::cout << "ok";
                }
             }
         }


        //******************************************

         //******************************************
         //elegxw yparksi connection: close
         keep_alive = (request.find("\nConnection: close", get_end_idx) == std::string::npos);

         //******************************************
        //is_dirty = false;

        //request_path = "/usr/local/var/www/72b.html";
    //}

    return 0;
}

QString HTTP_Header_parser::get_hostname_and_requested_path()
{
    if (hostname_and_requested_path_.isEmpty()){
        hostname_and_requested_path_ = hostname + request_uri;
    }
    return hostname_and_requested_path_;
}

std::vector<std::string> HTTP_Header_parser::split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}
