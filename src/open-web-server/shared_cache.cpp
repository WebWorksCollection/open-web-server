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

//Cache replacement policies
//https://en.wikipedia.org/wiki/Cache_replacement_policies

//Web Caching Basics: Terminology, HTTP Headers, and Caching Strategies
//https://www.digitalocean.com/community/tutorials/web-caching-basics-terminology-http-headers-and-caching-strategies

//13.4 Response Cacheability
//https://www.w3.org/Protocols/rfc2616/rfc2616-sec13.html#sec13.4


#include "shared_cache.h"
#include <QCoreApplication>
#include <iostream>

SharedCache::SharedCache()
{
    QString app_path = QCoreApplication::applicationDirPath();

    this->connect(&file_system_watcher,
                     SIGNAL(fileChanged(QString)),
                     this,
                     SLOT(slot_fileChanged(QString)));

    /*
    this->connect(&file_system_watcher_,
                     SIGNAL(directoryChanged(QString)),
                     this,
                     SLOT(slot_fileChanged(QString)));
*/
}

void SharedCache::slot_fileChanged(const QString &path)
{
    file_system_watcher.removePath(path);

    for (auto it = cache_.begin(); it != cache_.end(); it++){
        if (it->first.real_file_path == path){
            cache_.erase(it);
            break;
        }//if
    }//for

}
