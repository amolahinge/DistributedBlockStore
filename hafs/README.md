# RNBSS
For building
```mkdir -p cmake/build```
```cd cmake/build```
```cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..```
```make -j```
For running server
```./server --port=<PORT> --path=<PATH_TO_BLOCK_DIR> --role=p --address=<other_address>``` For Primary Server
```./server --port=<PORT> --path=<PATH_TO_BLOCK_DIR> --role=b --address=<other_address>``` For Backup Server


For running the basic client
```./basic_read_write``` Note that the address is hardcoded, be aware about that!


Recovery Testing
    1. Test case : primary receives write request and fails 
       Output : not persisted on either on primary & backup

``./recovery_testing --tc=primaryFail``
