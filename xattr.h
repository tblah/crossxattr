/*
Copyright (c) 2015  Tom Eccles
github.com/tblah

This is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XATTR_H
#define XATTR_H

#include <sys/types.h>

// this is always done in the user namespace so only the name of the attribute
// should be specified in attrname
ssize_t getAttr( const char* path, const char* attrname, void* data, size_t nbytes );
ssize_t setAttr( const char* path, const char* attrname, const void* data, size_t nbytes );
ssize_t deleteAttr( const char* path, const char* attrname );

// returns a linux style list but only for namespace user:
// name1\0name2\0name3\0name4...
ssize_t listAttrs( const char* path, void* data, size_t nbytes );

#endif
