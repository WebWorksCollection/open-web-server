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

#ifndef SHARED_CACHE_H
#define SHARED_CACHE_H

#include <mutex>
#include <unordered_map>
#include <QString>
#include <vector>
#include <qstring_hash_specialization.h>

class SharedCache
{
public:
    SharedCache();

    //public variables
    long long int max_file_size = 20971520; // [20971520 20MB, 1048576 1MB] megixto megethos arxeiou pou mporei na mpoei stin cache
    long long int cache_max_size = 419430400; //400MB megixto megethos cache
    long long int cache_current_size = 0;

    std::unordered_map<QString, std::vector<char>> cache_;

};

#endif // SHARED_CACHE_H
