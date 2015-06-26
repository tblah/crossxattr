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

#include "xattr.h"
#include <stdio.h>
#include <stdlib.h>
#include "errExit.h"
#include <string.h>

#define LISTATTRSBUF_SIZE 128
#define TEST_FILE "testFile"
#define TEST_ATTR_NAME "ctest_testing_attr"
#define TEST_ATTR_VAL "testing"

// TODO: does not check at the beginning if there is already an attribute on
// the file called TEST_ATTR_NAME

int main( void ) {
    // test that we can list the number of attributes
    void* data = malloc( LISTATTRSBUF_SIZE );
    if ( data == NULL ) {
        errExit( "test: malloc listAttrs buffer" );
    }

    // pad data with NULL to make valgrind happy
    for ( size_t i = 0; i < LISTATTRSBUF_SIZE; i++ )
        ((char*) data)[i] = '\0';

    ssize_t numAttrs = listAttrs( TEST_FILE, data, LISTATTRSBUF_SIZE );
    if ( numAttrs < 0 )
        errExit( "test: listAttrs" );

    free( data );

    // now try adding an attribute
    char testAttrVal[] = TEST_ATTR_VAL;
    setAttr( TEST_FILE, TEST_ATTR_NAME, testAttrVal, strlen( TEST_ATTR_VAL ) + 1 );

    // and try reading it back again
    getAttr( TEST_FILE, TEST_ATTR_NAME, testAttrVal, strlen( TEST_ATTR_VAL ) + 1 );
    
    if ( strcmp( testAttrVal, TEST_ATTR_VAL ) != 0 ) {
        fprintf( stderr, "The value read back was not the same as the one we put in\n" );
        fprintf( stderr, "We read back \"%s\" and we put in \"%s\"\n", testAttrVal, TEST_ATTR_VAL );
        exit( EXIT_FAILURE );
    }

    // now delete the attribute we added
    deleteAttr( TEST_FILE, TEST_ATTR_NAME );

    // check that we are back to the origional number of attributes
    ssize_t newNumAttrs = listAttrs( TEST_FILE, data, LISTATTRSBUF_SIZE );
    if ( numAttrs < 0 )
        errExit( "test: listAttrs" );

    if ( newNumAttrs != numAttrs ) {
        errExit( "test: we are not back to the origional number of attributes" );
    }

    printf( "All tests worked :-)\n" );

    return EXIT_SUCCESS;
}
