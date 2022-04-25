libwsclient
===========
original work: https://github.com/payden/libwsclient
util reference: https://www.rfc-editor.org/rfc/rfc6455#section-5.5.3
modifications:
    - test.c     : personalzied the test.c file
    - wsclient.h : change typedef structs nomenclature
    - wsclient.h : added some functions declarations
    - wsclient.h : added void *ref_struct to struct wsclient (just a simple general pointer)
    - wsclient.c : change typedef structs nomenclature
    - wsclient.c : added option code argument in function libwsclient_send()
    - wsclient.c : added control frame 0x9(ping)
    - wsclient.c : fix memory leaks as sugested in https://github.com/payden/libwsclient/issues/16
    - moved global variables (libwsclient_flags,errors) from wsclient.h to wsclient.c to avoid atomic or inline

===========
WebSocket client library for C

This library abstracts away WebSocket protocol framing for
client connections.  It aims to provide a *somewhat* similar
API to the implementation in your browser.  You create a new
client context and create callbacks to be triggered when
certain events occur (onopen, onmessage, onclose, onerror).

Your best bet for getting started is to look at test.c which shows
how to connect to an echo server using libwsclient calls.

Also, to install:

./autogen.sh

./configure && make && sudo make install

Then link your C program against wsclient:

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

Compile test:

'gcc -g -O2 -o test test.c -lwsclient'

Run test:

./test


