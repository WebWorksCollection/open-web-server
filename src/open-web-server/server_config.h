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

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "server_config_virtualhost.h"
#include "server_config_parser_base.h"
#include "qstring_hash_specialization.h"
#include <QString>
#include <QFile>
#include "http_header_parser.h"
#include "clientsession.h"

class ServerConfig
{
public:
    ServerConfig();

    //variables
    std::unordered_map<QString, ServerConfigVirtualHost> server_config_map;
    ServerConfigParserBase *server_config_parser;

    //methods
    bool parse_config_file(QString filename);
    bool is_valid_requested_hostname(HTTP_Header_parser &http_header_parser);
    bool index_exists(ClientSession &client_session, QFile & file_io);
};

#endif // SERVER_CONFIG_H
