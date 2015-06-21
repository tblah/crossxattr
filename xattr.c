/*
Copyright (c) 2015 Tom Eccles
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
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

// exit on error function
#include <stdlib.h>
#include <stdio.h>

void errExit( const char* errMsg ) {
    fprintf( stderr, "Quitting on extended attribute error %s\n", errMsg );
    exit( EXIT_FAILURE );
}

// wrappers for the systemcalls so I can use them in go

#define ERR_STR_SIZE 100

// *BSD support
#if ( defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined __DragonFly__ )

#include <sys/types.h>
#include <sys/extattr.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/vnode.h>

void checkReturnValue( const char* callerName, ssize_t ret ) {
    char errStr[ERR_STR_SIZE];

    if ( errno == EFAULT ) {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "EFAULT: invalid arguements to %s", callerName ); 
        if ( snprintfRet < 0 )
            errExit( "sprintf" );

        errExit( errStr );

    } else if ( errno == ENAMETOOLONG ) {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "ENAMETOOLONG: from %s", callerName );
        if ( snprintfRet < 0 )
            errExit( "sprintf" );

        errExit( errStr );

    } else if ( errno == ENOATTR ) {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "Requested non-existant extended attribute in %s", callerName );
        if ( snprintfRet < 0 )
            errExit( "sprintf" );

        fprintf( stderr, "%s\n", errStr );

    } else if ( ( errno == ENOTDIR ) || ( ret == ENAMETOOLONG) || ( ret == ENOENT ) || ( ret == EACCES ) ) {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "There is a problem with the path given to %s", callerName );
        if ( snprintfRet < 0 )
            errExit( "snprintf" );

        errExit( errStr );
    } else {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "Some other error occured. See the man pages (chapter 2) for stat and for the linux system call. This is from %s and errno was %i", callerName, errno );
        if ( snprintfRet < 0 )
            errExit( "snprintf" );

        errExit( errStr );
    }
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
    // TODO: fix output to conform with independent form as specified in the
    // headerfile
    ssize_t ret = extattr_list_file( path, EXTATTR_NAMESPACE_USER, data, nbytes );

    checkReturnValue( "listAttrs", ret );

    return ret;
}

#endif

// GNU+Linux 
#ifdef __gnu_linux__

#include <sys/types.h>
#include <attr/xattr.h>
#include <errno.h>

void checkReturnValue( const char* callerName, ssize_t ret ) {
    if ( ret != -1 )
        return;

    char errStr[ERR_STR_SIZE];
    
    if ( errno == ENOATTR ) {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "The named attribute does not exist or you have no access to it (from %s)",
                callerName );
        if ( snprintfRet < 0 )
            errExit( "snprintf" );

        fprintf( stderr, "%s\n", errStr );

    } else if ( errno == ENOTSUP ) {
        errExit( "You forgot to mount the filesystem with extended attributes enabled" );

    } else if ( errno == ERANGE ) {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "The size of the buffer is insufficient to hold the result (from %s)",
                callerName );
        if (snprintfRet < 0 )
            errExit( "sprintf" );

        errExit( errStr );
    } else if ( ( errno == EDQUOT ) || ( errno == ENOSPC ) ) {
        errExit( "There is insufficient space on the filesystem or in the user filesystem quota to perform extended attribute operation\n" );
    } else if ( errno == EACCES )
        errExit( "You do not have search permission on one of the directories in the specified path\n" );
    else {
        int snprintfRet = snprintf( errStr, ERR_STR_SIZE, "Some other error occured. See the man pages (chapter 2) for stat and for the linux system call. This is from %s and errno was %i", callerName, errno );
        if ( snprintfRet < 0 )
            errExit( "snprintf" );

        errExit( errStr );
    }
}
// TODO: add the user namespace prefix to all of the names
ssize_t getAttr( const char* path, const char* attrname, void* data, size_t nbytes ) {
    ssize_t ret = getxattr( path, attrname, data, nbytes );

    checkReturnValue( "getAttr", ret );

    return ret;
}

ssize_t setAttr( const char* path, const char *attrname, const void* data, size_t nbytes ) {
    // flags = 0 means it will be created if it does not exist or replaced if
    // it does
    ssize_t ret = setxattr( path, attrname, data, nbytes, 0 );

    checkReturnValue( "setAttr", ret );

    return ret;
}

ssize_t deleteAttr( const char* path, const char* attrname ) {
    ssize_t ret = removexattr( path, attrname );

    checkReturnValue( "deleteAttr", ret );

    return ret;
}

ssize_t listAttrs( const char* path, void* data, size_t nbytes ) {
    // TODO: remove non-user namespace list items and remove the namespace name
    // specifier
    ssize_t ret = listxattr( path, (char*) data, nbytes );

    checkReturnValue( "listAttrs", ret );

    return ret;
}

#endif
