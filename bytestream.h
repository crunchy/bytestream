// bytestream.h
//
// Purpose: Utilities for creating bytestream-able data
// Author:  Luke van der Hoeven
// Date:    7/5/12

// Protocol definition

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "vendor/tpl.h"

#define SC_TimeBase    1000.0

#define STOP  0x01
#define START 0x02
#define MOUSE 0x03
#define VIDEO 0x04

#define MAX_BODY_SIZE 128

typedef uint32_t sc_time;
static char TPL_STRUCTURE[] = "S($(vu)su)";

typedef struct {
    uint16_t x, y;
} sc_mouse_coords;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} sc_frame_rect;

typedef struct {
  uint16_t type;
  sc_time timestamp;
} sc_bytestream_header;

typedef struct {
  sc_bytestream_header header;
  char *body;
  uint32_t body_size;
} sc_bytestream_packet;

sc_bytestream_packet sc_bytestream_put_start(int fd);
sc_bytestream_packet sc_bytestream_put_stop(int fd);
sc_bytestream_packet sc_bytestream_put_mouse_data(int fd, sc_mouse_coords coords);
sc_bytestream_packet sc_bytestream_put_frame(int fd, char *data);

void sc_bytestream_get_event();
void sc_bytestream_get_event_header();
sc_mouse_coords sc_bytestream_get_mouse_data(int fd);
void sc_bytestream_get_frame();


char *append_uint16(char *output, char *end, uint16_t value);
uint16_t decode_uint16(const char *data);

sc_bytestream_header create_header(uint8_t event);
void serialize_packet(int fd, sc_bytestream_packet packet);
sc_bytestream_packet deserialize_packet(int fd);
