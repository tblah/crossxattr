/*
Crossxattr
==========

Wrappers for xattr related system calls so they have a common interface across GNU+Linux & FreeBSD

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

#include "errExit.h"

#define ERR_STR_SIZE 200

// FreeBSD support
#if ( defined __FreeBSD__ )

#include <sys/types.h>
#include <sys/extattr.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/vnode.h>
#include <stdio.h>

void checkReturnValue( const char* callerName, ssize_t ret ) {
	/*
	 Errno documentation can be found online here:
	 https://www.freebsd.org/cgi/man.cgi?query=errno&apropos=0&sektion=0&manpath=FreeBSD+10.2-RELEASE&arch=default&format=html
	 They can of course also be accessed via man on FreeBSD
	*/

	if ( ret >= 0 ) {
		return;
	}

    char errStr[ERR_STR_SIZE];

	int snprintfRet = 0;

	switch ( errno ) {
	case EFAULT:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "EFAULT in %s", callerName );
	break;
	
	case ENAMETOOLONG:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENAMETOOLONG: in %s", callerName );
	break;
	
	case ENOATTR:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENOATTR in %s", callerName );
	break;

	case ENOTDIR:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENOTDIR in %s", callerName );
	break;

	case ENOENT:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENOENT in %s", callerName );
	break;

	case EACCES:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "EACCES in %s", callerName );
	break;

	default:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "Unknown error %i in %s", errno, callerName);
	}

	if ( snprintfRet < 0 ) {
		errExit( "snprintf");
	}

	errExit( errStr );
}

ssize_t getAttr( const char* path, const char* attrname, void* data, size_t nbytes ) {
    ssize_t ret = extattr_get_file( path, EXTATTR_NAMESPACE_USER, attrname, data, nbytes );

    checkReturnValue( "getAttr", ret );

    return ret;
}

ssize_t setAttr( const char* path, const char *attrname, const void* data, size_t nbytes ) {
    ssize_t ret = extattr_set_file( path, EXTATTR_NAMESPACE_USER, attrname, data, nbytes );

    checkReturnValue( "setAttr", ret );

    return ret;
}

ssize_t deleteAttr( const char* path, const char* attrname ) {
    ssize_t ret = extattr_delete_file( path, EXTATTR_NAMESPACE_USER, attrname );

    checkReturnValue( "deleteAttr", ret );

    return ret;
}

ssize_t listAttrs( const char* path, void* data, size_t nbytes ) {
    /* 
	 extattr_list_file() returns a list of attributes present in the requested namespace.
	 Each list entry consists of a single byte containing the length of the attribute name.
	 The attribute name is not terminated by ASCII 0 (nul).
	*/
    ssize_t ret = extattr_list_file( path, EXTATTR_NAMESPACE_USER, data, nbytes );

    checkReturnValue( "listAttrs", ret );

	// Do the string manipulation
	char* bitstring = data;// Cast the void pointer to a string
	int len = (int) bitstring[0];// Bootstrap the first pointer
	for ( int i = 1; i < nbytes; i++ ) {
		if ( i == len ) {
			// Set the position of the next pointer
			len = (int) bitstring[i];
			bitstring[i] = '\0';
		}
		bitstring[i-1] = bitstring[i];
	}
    return ret;
}

#endif // FreeBSD

// GNU+Linux 
#ifdef __gnu_linux__

#include <sys/types.h>
#include <attr/xattr.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void checkReturnValue( const char* callerName, ssize_t ret ) {

    if ( ret >= 0 ) {
        return;
	}

    char errStr[ERR_STR_SIZE];
	int snprintfRet = 0;
    
	switch ( errno ) {
	case ENOATTR:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENOATTR in %s", callerName );
	break;

	case ENOTSUP:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENOSUP in %s", callerName );
	break;

	case ERANGE:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ERANGE in %s", callerName );
	break;

	case EDQUOT:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "EDQUOT in %s", callerName );
	break;

	case ENOSPC:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENOSPC in %s", callerName );
	break;

	case EACCES:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "EACCES in %s", callerName );
	break;

	default:
		snprintfRet = snprintf( errStr, ERR_STR_SIZE, "Unknown error %i in %s", errno, callerName);
	}

	if ( snprintfRet < 0 ) {
		errExit( "snprintf" );
	}

	errExit( errStr );
}

char* addNamespacePrefix( const char* str ) {
    // adds "user." to the beginning of the name to specify the namespace
    
    // strlen( "user." ) == 5
    char* ret = ( char* ) malloc( strlen( str ) + 5 );
    if ( ret == NULL )
        errExit( "allocating namespace prefix memory" );

    int snprintfRet = snprintf( ret, strlen( str ) + 5 , "user.%s", str );
    if ( snprintfRet < 0 )
        errExit( "snprintf in addNamespacePrefix" );

    return ret;
}

ssize_t getAttr( const char* path, const char* attrname, void* data, size_t nbytes ) {
    char* name = addNamespacePrefix( attrname );
    ssize_t ret = getxattr( path, name, data, nbytes );

    free( name );
    name = NULL;

    checkReturnValue( "getAttr", ret );

    return ret;
}

ssize_t setAttr( const char* path, const char *attrname, const void* data, size_t nbytes ) {
    char* name = addNamespacePrefix( attrname );
    // flags = 0 means it will be created if it does not exist or replaced if
    // it does
    ssize_t ret = setxattr( path, name, data, nbytes, 0 );

    free( name );
    name = NULL;

    checkReturnValue( "setAttr", ret );

    return ret;
}

ssize_t deleteAttr( const char* path, const char* attrname ) {
    char* name = addNamespacePrefix( attrname );
    ssize_t ret = removexattr( path, name );

    free( name );
    name = NULL;

    checkReturnValue( "deleteAttr", ret );

    return ret;
}

char* seekToInterestingPart( const char* s, size_t* entriesRemaining ) {
    // seeks past just the namespace part if its a usernamespace string,
    // otherwise it skips to the next one and tries again

    if ( (*entriesRemaining) < 1 )
        return NULL;

    (*entriesRemaining)--;

    if ( strncmp( "user.", s, 5 ) == 0 ) {
        // this is in the user namespace so return the address of the name
        return strchr( s, '.' ) + 1;
    } else {
        // this is not in the user namespace so try the next entry
        return seekToInterestingPart( s + strlen( s ) + 1, entriesRemaining ); 
    }

    return NULL;
}

ssize_t numCharInStr( char c, char* s, ssize_t len ) {
    ssize_t ret = 0;

    for ( ssize_t i = 0; i < len; i++ )
        if ( s[i] == c )
            ret++;

    return ret;
}

ssize_t listAttrs( const char* path, void* data, size_t nbytes ) {
    ssize_t listxattrRet = listxattr( path, (char*) data, nbytes );

    checkReturnValue( "listAttrs", listxattrRet);

    if ( listxattrRet < 1 )
        errExit( "listxattrRet < 1" );

    ssize_t numEntries = numCharInStr( '.', data, listxattrRet );

    // remove non-user namespace list items and remove the namespace name
    // specifier
    
    // first allocate a list of pointers to each string. Worst case every
    // attribute which is found is in the USER namespace so we need a
    // dynamically array which is this long
    char** usefulBits = (char**) malloc( sizeof(char*) * numEntries );
    if ( usefulBits == NULL )
        errExit( "usefulBits malloc in listAttrs" );

    // fill usefulBits with NULL for now
    for ( ssize_t i = 0; i < numEntries; i++ )
        usefulBits[i] = NULL;

    // now put in the relevant pointers
    size_t numEntriesRemaining = numEntries;
    usefulBits[0] = seekToInterestingPart( (char*) data, &numEntriesRemaining );
    for ( ssize_t i = 1; i < numEntries; i++ ) { // don't do this too many times
        // returns NULL if we have run out
        usefulBits[i] = seekToInterestingPart( usefulBits[i-1] + strlen(usefulBits[i-1]) + 1, &numEntriesRemaining );
    }

    // work out how many entries we have
    ssize_t numInterestingEntries = 0;
    for ( ssize_t i = 0; i < numEntries; i++ ) {
        if ( usefulBits[i] == NULL ) {
            // this is all of them
            break;
        } else {
            numInterestingEntries++;
        }
    }

    // now we overwrite data with the newly edited list. This will definitely
    // fit because we have removed things from what was previously in data
    // for the same reason we can be sure that we won't be overwriting ourself

    // these for loops could all be combined however this form is better for readability
    char* currPos = (char*) data;
    for ( ssize_t i = 0; i < numInterestingEntries; i++ ) {
        size_t size = strlen( usefulBits[i] );
        strcpy( currPos, usefulBits[i] );
        currPos += ( size + 2 );
    }
        
    free( usefulBits );
    return numInterestingEntries;
}

#endif // GNU+Linux
