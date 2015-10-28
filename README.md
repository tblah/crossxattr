# crossxattr
Cross platform extended attributes C code for another project.

This project is unfinished.

Support is currently implemented for FreeBSD and GNU+Linux.

## Functions
All extended attributes are operated on in the USER namespace. Extended attributes of links are not planned to be supported (although there is no reason why not).

``` C
ssize_t getAttr( const char* path, const char* attrname, void* data, size_t nbytes );
ssize_t setAttr( const char* path, const char* attrname, const void* data, size_t nbytes );
ssize_t deleteAttr( const char* path, const char* attrname );
ssize_t listAttrs( const char* path, void* data, size_t nbytes );
```

- path - path to the file to be operated upon
- attrname - the name of the attribute, not specifying the namespace
- nbytes - the size of the buffer data
- data - the buffer for the result of the operation

- For getAttr data is set to the value of the specified attribute,
- For setAttr data is the value to which the specified attribute is set,
- For listAttrs data is set to be a NULL seperated list of the names of the attributes which are in the USER namespace of the file: e.g.
```
name1\0name2\0name3\0...
```

### Return Values
They return -1 on failure (everything other than attribute does not exist is
caught internally).

Otherwise they return whatever the relevant syscall returns. 

The one exception to this is listAttrs. This returns the number of entries in
data.
