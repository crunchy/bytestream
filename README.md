What
----

A simple bytestream lib for interaction between our screenshare capture component and x264 encoder/rtmp streamer for licensing purposes.

API
---

Packet
======

Each packets consists of a header (struct), a body (tpl_bin) and a body size (int).
The body is a tpl binary stream structure, which gets cast back into whatever form it originally took (usually mouse data or frame data).

Header
======

A header consists of an event type and a timestamp. The timestamp is simply the time the packet/header is created (using time()).

There are four types of events:

`START, STOP. MOUSE, FRAME`

Start/Stop take no params, they simply create a packet with a header

Data Types
==========

* `sc_mouse_coords` : contains a pair of uint16_t, x and y.

* `sc_frame_rect` : simple c rect structure, x, y (origin) and width, height.
all members are uint16_t.

* `sc_bytestream_header` : type and timestamp (as described above)

* `sc_frame` : byte pointer framePtr to addressable data, size int, the size of the data.

* `sc_bytestream_packet` : header (the above) tpl_bin which holds the body of data (usually a sc_frame or sc_mouse_coords)


Functions
---------

The following methods handle writing and reading the packets across a given file descriptor (fd). This could be a socket, a file handle, or other i/o stream. Each method takes the fd as its first parameter. `start/stop` do not take any other parameters. The mouse data writer takes an `sc_mouse_coords` struct and the frame writer takes an `sc_frame`. Each "put" method also returns the packet it created.

The get methods are really only used in the case of a mouse or frame event. They return their respective data structures. Start/stop will return a tpl_bin but the address (addr) is a NULL pointer and has a size (sz) of zero.

`sc_bytestream_get_event/_header` return the raw header or packet information.

```C
sc_bytestream_packet sc_bytestream_put_start(int fd)
sc_bytestream_packet sc_bytestream_put_stop(int fd)
sc_bytestream_packet sc_bytestream_put_mouse_data(int fd, sc_mouse_coords coords)
sc_bytestream_packet sc_bytestream_put_frame(int fd, sc_frame frame)

sc_mouse_coords sc_bytestream_get_mouse_data(int fd)
sc_frame sc_bytestream_get_frame(int fd)

sc_bytestream_packet sc_bytestream_get_event(int fd)
sc_bytestream_header sc_bytestream_get_event_header(int fd)
```


The following methods are also available, but are not usually needed outside of the library, as they are all used to build or decode the packets.

```
sc_bytestream_header create_header(uint8_t event);
void serialize_packet(int fd, sc_bytestream_packet packet);
sc_bytestream_packet deserialize_packet(int fd);
```

Testing
-------

First compile the tpl.c file with gcc

`gcc -c vendor/tpl.c`

then you can simply run `make` to compile the test and run the tests using the script it builds.

./bytesream_spec

Dependencies
----

This library is tested via googletest (http://code.google.com/p/googletest/) and depends on the tpl serialization library (http://tpl.sourceforge.net/userguide.html).