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

// function definitions inside the source file because they are not intended to be used externally
void setup();// create test file
void tear_down();// delete the test file name
ssize_t listattr_numattr();// test that we can list the number of attributes
void setattr_testattr();// attempt to add the test attr
ssize_t setattr_create();// attempt to create an attribute and check that an attribute has been created
ssize_t getattr_value();// compare attr value received with attr value set
ssize_t deleteattr_numattr();// delete an attr and check the attr is deleted

int main( void ) {

    setup();

	int passed = 0;
   	int	run = 0;

	int ret = listattr_numattr();
	run++;
	if ( ret < 0 ) {
		fprintf( stdout, "test: an error in listAttr\n" );
	} else if ( ret > 0 ) {
		fprintf( stdout, "test: listAttr returning non 0 when no attrs have been created\n" );
	} else {
		passed++;
	}

	run++;
	if ( setattr_create() == EXIT_FAILURE ) {
		fprintf( stdout, "test: setattr_create() failed\n" );
	} else {
		passed++;
	}

	run++;
	if ( getattr_value() == EXIT_FAILURE ) {
		fprintf( stdout, "test: getattr_value failed\n" );
	} else {
		passed++;
	}

	run++;
   	if ( deleteattr_numattr() == EXIT_FAILURE ) {
		fprintf( stdout, "test: deleteattr_numattr failed\n" );
	} else {
		passed++;
	}

	fprintf( stdout, "%i%% of tests passed\n", (passed / run) * 100 );

    return EXIT_SUCCESS;
}

ssize_t listattr_numattr() {

	void* data = malloc( LISTATTRSBUF_SIZE );
	
    if ( data == NULL ) {
        errExit( "test: malloc listAttrs buffer" );
    }

    // pad data with NULL to make valgrind happy
    for ( size_t i = 0; i < LISTATTRSBUF_SIZE; i++ ){
        ((char*) data)[i] = '\0';
	}

    ssize_t numAttrs = listAttrs( TEST_FILE, data, LISTATTRSBUF_SIZE );
 
    free( data );// tidy up our allocation before we exit the function
	data = NULL;

 	if ( numAttrs < 0 ) {
		return EXIT_FAILURE;
	}

	return numAttrs;
}

void setup() {

	if ( fclose( fopen( TEST_FILE, "w+" ) ) == EOF ){
		printf( "Error creating test file\n" );
		exit( EXIT_FAILURE );
	}// Create the test file
	
	atexit( tear_down );// register our tear down function

}

void tear_down()  {

	remove( TEST_FILE );// delete the file name

}

void setattr_testattr() {

	char testAttrVal[] = TEST_ATTR_VAL;
    setAttr( TEST_FILE, TEST_ATTR_NAME, testAttrVal, strlen( TEST_ATTR_VAL ) + 1 );

}

ssize_t setattr_create() {

	setattr_testattr();

	if ( listattr_numattr() != 1 ) {
		return EXIT_FAILURE;
	}

	deleteAttr( TEST_FILE, TEST_ATTR_NAME );

	return EXIT_SUCCESS;
}

ssize_t getattr_value() {
	
	setattr_testattr();

	char testAttrVal[] = TEST_ATTR_VAL;
	getAttr( TEST_FILE, TEST_ATTR_NAME, testAttrVal, strlen( TEST_ATTR_VAL ) + 1 );
    
	deleteAttr( TEST_FILE, TEST_ATTR_NAME );

    if ( strcmp( testAttrVal, TEST_ATTR_VAL ) != 0 ) {
        return EXIT_FAILURE;
    } 
	
	return EXIT_SUCCESS;
}

ssize_t deleteattr_numattr() {

	setattr_testattr();

	deleteAttr( TEST_FILE, TEST_ATTR_NAME );

	if ( listattr_numattr( TEST_FILE ) != 0 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
