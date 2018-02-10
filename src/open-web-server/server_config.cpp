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

#include "server_config.h"

ServerConfig::ServerConfig()
{

}

bool ServerConfig::parse_config_file(QString filename)
{
    return server_config_parser->parse_config_file(filename, server_config_map);
}

bool ServerConfig::is_valid_requested_hostname(HTTP_Header_parser &http_header_parser)
{
    //anazitisi ean to hostname yparxei sto map me ta hostnames
    auto it = server_config_map.find(http_header_parser.hostname);

    if (it != server_config_map.end()){
        //to hostname vrethike opote lamvanw to path tou kai to ennonw me to
        //path tou resource pou zitithike
        http_header_parser.absolute_hostname_and_requested_path_ = it->second.DocumentRoot + http_header_parser.request_uri;
        http_header_parser.server_config_map_it = std::move(it);
        return true;
    } else {
        //to hostname den vrethike opote elegxw ean yparxei default (*) hostname
        it = server_config_map.find("*");
        if (it != server_config_map.end()){
            std::vector<QString> directoryIndexes = it->second.directoryIndexes;
            http_header_parser.absolute_hostname_and_requested_path_ = it->second.DocumentRoot + http_header_parser.request_uri;
            http_header_parser.server_config_map_it = std::move(it);
            return true;
        }
    }

    //den vrethike, opote epistrefw keno string
    return false;
}

//exontas to virtual host pou zitithike kai to path, psaxnw na
//vrw ean yparxei arxeio index wste na apostalei
bool ServerConfig::index_exists(ClientSession &client_session, QFile &file_io)
{
    //lamvanw to array me ta indexes apo to iterator pou proekipse
    //kalontas pio prin tin is_valid_requested_hostname
    std::vector<QString> directoryIndexes = client_session.request.server_config_map_it->second.directoryIndexes;

    //krataw ena antigradw tis pliris diadromis pou zitithike, wste na tin epanaferew
    //ean den vrethei index file.
    QString absolute_host_and_uri = client_session.request.absolute_hostname_and_requested_path_;
    bool ends_with_slash = client_session.request.absolute_hostname_and_requested_path_.endsWith("/");

    //dokimazw kathe index arxeio, prosthetontas to sto yparxon absolute path
    for (const QString &index_filename : directoryIndexes){
        ends_with_slash ? client_session.request.absolute_hostname_and_requested_path_ += index_filename :
                client_session.request.absolute_hostname_and_requested_path_ = client_session.request.absolute_hostname_and_requested_path_ + "/" + index_filename;
        file_io.setFileName(client_session.request.absolute_hostname_and_requested_path_);
        if (file_io.open(QFileDevice::ReadOnly) == true) {
            //ean vrethike
                 ends_with_slash ? client_session.request.request_uri += index_filename :
                         client_session.request.request_uri = client_session.request.request_uri + "/" + index_filename;
            return true;
        }
        //epanaferw to absolute path opws itan
        client_session.request.absolute_hostname_and_requested_path_ = absolute_host_and_uri;
    }

    return false;
}


