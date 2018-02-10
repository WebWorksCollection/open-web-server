# * ***********************************************************************
# * Open Web Server
# * (C) 2018 by Yiannis Bourkelis (hello@andama.org)
# *
# * This file is part of Open Web Server.
# *
# * Open Web Server is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * Open Web Server is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with Open Web Server.  If not, see <http://www.gnu.org/licenses/>.
# * ***********************************************************************/

QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#debug: DEFINES += MY_DEBUG

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    poll_server.cpp \
    message_composer.cpp \
    html_messagecomposer.cpp \
    clientsession.cpp \
    http_message_dispatcher.cpp \
    http_header_parser.cpp \
    http_response_templates.cpp \
    shared_cache.cpp \
    server_config.cpp \
    server_config_parser.cpp \
    server_config_virtualhost.cpp \
    server_config_parser_base.cpp

HEADERS += \
    poll_server.h \
    message_composer.h \
    html_messagecomposer.h \
    clientsession.h \
    helper_functions.h \
    http_message_dispatcher.h \
    http_header_parser.h \
    http_response_templates.h \
    shared_cache.h \
    server_config.h \
    server_config_parser.h \
    server_config_virtualhost.h \
    server_config_parser_base.h \
    qstring_hash_specialization.h
