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

#include <QCoreApplication>
#include "poll_server.h"
#include "html_messagecomposer.h"
#include "signal.h"
#include "server_config_parser.h"
#include "server_config.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    #ifndef WIN32
    //gia na mi prokaleitai crash otan paw na grapsw se socket pou exei kleisei
    //http://stackoverflow.com/questions/108183/how-to-prevent-sigpipes-or-handle-them-properly
    signal(SIGPIPE, SIG_IGN);
    #endif

    ServerConfig server_config;
    ServerConfigParser server_config_parser;
    server_config.server_config_parser = &server_config_parser;
    server_config.parse_config_file("/Users/yiannis/Projects/open-web-server-github/open-web-server/src/server-config/server_config.json");


    PollServer pollserver;

    HTML_MessageComposer html_msg_composer;
    html_msg_composer.http_msg_dispatcher_.server_config_ = &server_config;
    pollserver.msgComposer = &html_msg_composer;
    //pollserver.msgComposer.reset(&html_msg_composer);

    pollserver.start(12343, PollServer::IPv4);

    return a.exec();
}
