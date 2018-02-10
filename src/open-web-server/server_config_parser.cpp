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
#include <QJsonDocument>
#include <typeinfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QVector>

ServerConfigParser::ServerConfigParser()
{

}

bool ServerConfigParser::parse_config_file(const QString &filename, std::unordered_map<QString, ServerConfigVirtualHost> &server_config_map)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
           return false;

    QJsonDocument json_doc(QJsonDocument::fromJson(file.readAll()));

    QJsonArray http_settings = json_doc["http"].toArray();

    QJsonArray server_indexes = json_doc["server"]["indexes"].toArray();


    for (int i = 0; i < http_settings.size(); i++){
        QJsonArray srv = http_settings.at(i)["virtual_host"]["server_name"].toArray();

        QJsonArray listen_array = http_settings.at(i)["virtual_host"]["listen"].toArray();
        for (int listen_idx = 0; listen_idx < listen_array.size(); listen_idx++){
            for (int j = 0; j < srv.size(); j++){
                ServerConfigVirtualHost vhost;
                vhost.ServerName.push_back(srv.at(j).toString());
                vhost.DocumentRoot = http_settings.at(i)["virtual_host"]["document_root"].toString();

                //directory indexes
                for (auto idx_itm : json_doc["server"]["indexes"].toArray()){
                    vhost.directoryIndexes.push_back(idx_itm.toString());
                }

                //kataxwrisi tou vhost sto unordered map
                QString server_and_port = srv.at(j).toString() != "*" ? srv.at(j).toString() + ":" + listen_array.at(listen_idx)["port"].toString() : srv.at(j).toString();
                server_config_map.emplace(std::make_pair(server_and_port, vhost));
            }//server_name array for loop end
        }//listen array for loop end
    }

    return true;
}
