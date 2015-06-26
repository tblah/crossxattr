/*
Copyright (c) 2015 Tom Eccles
github.com/tblah/crossxattr

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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

// exit on error function
void errExit( const char* errMsg ) {
    int oldErrno = errno;
    fprintf( stderr, "Quitting on error: %s\n", errMsg );
    fprintf( stderr, "Errno was %i\n", oldErrno );
    exit( EXIT_FAILURE );
}

