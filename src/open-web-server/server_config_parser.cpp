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

#include "server_config_parser.h"
#include <QFile>
#include <iostream>
#include <fstream>

#include "json.hpp"

ServerConfigParser::ServerConfigParser()
{

}

bool ServerConfigParser::parse_config_file(const QString &filename, std::unordered_map<QString, ServerConfigVirtualHost> &server_config_map)
{
    std::ifstream i(filename.toStdString());
    if (!i.is_open()) return false;

    server_config_map.clear();

    nlohmann::json j;
    i >> j;

    auto http_settings_ = j["http"];

    for (auto http_setting : http_settings_){
    //for (size_t i = 0; i < http_settings.size(); i++){
        auto vhost_hostnames = http_setting["virtual_host"]["server_name"];

        for (auto listen__ : http_setting["virtual_host"]["listen"]){
            for (size_t vhost_hostnames_idx = 0; vhost_hostnames_idx < vhost_hostnames.size(); vhost_hostnames_idx++){

                QString port__ = QString::fromStdString(listen__["port"].get<std::string>());

                ServerConfigVirtualHost vhost;
                QString vhost_name = QString::fromStdString(vhost_hostnames.at(vhost_hostnames_idx).get<std::string>());
                vhost.ServerName.push_back(vhost_name);
                vhost.DocumentRoot = QString::fromStdString(http_setting["virtual_host"]["document_root"].get<std::string>());

                //directory indexes
                for (auto idx_itm : j["server"]["indexes"]){
                    vhost.directoryIndexes.push_back(QString::fromStdString(idx_itm.get<std::string>()));
                }

                //kataxwrisi tou vhost sto unordered map
                QString server_and_port = vhost_name != "*" ? vhost_name + ":" + port__ : vhost_name;
                server_config_map.emplace(std::make_pair(server_and_port, vhost));
            }

        }//for listen_array
    }//for http_settings

    return true;
}
