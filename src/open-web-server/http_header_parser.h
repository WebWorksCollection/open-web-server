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

#ifndef HTTP_HEADER_PARSER_H
#define HTTP_HEADER_PARSER_H

#include <vector>
#include <QString>
#include <string>
#include "qstring_hash_specialization.h"
#include "server_config_virtualhost.h"


class HTTP_Header_parser
{
public:
    HTTP_Header_parser();
    HTTP_Header_parser(const std::vector<char> &__request_header);

    std::vector<char> request_header;//periexei to HTTP request
    QString request_uri;//to uri pou zitithike
    QString hostname;//to hostname pou zitithike
    QString absolute_hostname_and_requested_path_;//to path pou zitithike, symfwna me to maping tou hostname ws topiko path p.c. /users/web/wwwroot/example.com/public_html
    qint64 requested_file_size = 0;
    unsigned long long int range_from_byte = 0;
    unsigned long long int range_until_byte = 0;
    bool has_range_header = false;
    bool keep_alive = true; //ean sto HTTP request exei zitithei keep-alive
    std::unordered_map<QString, ServerConfigVirtualHost>::iterator server_config_map_it;
    //std::vector<std::string> headers;

    int parse();
    //bool is_dirty = true;

    QString get_hostname_and_requested_path();
private:
    //variables
    QString hostname_and_requested_path_;//o syndiasmos tou hostname pou zitithike mazi me to request_uri p.x. example.com/image.jpg

    //methods
    std::vector<std::string> split(const std::string& s, char delimiter);

};

#endif // HTTP_HEADER_PARSER_H
