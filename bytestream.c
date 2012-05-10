// bytestream.c
//
// Author:  Luke van der Hoeven
// Date:    7/5/12

#include <time.h>
#include "bytestream.h"

sc_bytestream_packet sc_bytestream_put_start(int fd) {
  sc_bytestream_packet packet = {create_header(START), NULL, 0};

  serialize_packet(fd, packet);
  return packet;
}

sc_bytestream_packet sc_bytestream_put_stop(int fd) {
  sc_bytestream_packet packet = {create_header(STOP), NULL, 0};

  serialize_packet(fd, packet);
  return packet;
}

sc_bytestream_packet sc_bytestream_put_mouse_data(int fd, sc_mouse_coords coords) {
  char buffer[36], *enc = buffer, *end = buffer + sizeof(buffer);

  enc = append_uint16(enc, end, coords.x);
  append_uint16(enc, end, coords.y);
  *enc++ = '\0';  // add null term

  sc_bytestream_packet packet = {create_header(MOUSE), enc, strlen(enc)};
  serialize_packet(fd, packet);

  return packet;
}

sc_mouse_coords sc_bytestream_get_mouse_data(int fd) {
  sc_bytestream_packet packet = deserialize_packet(fd);
  uint16_t x,y;

  packet.body--;    // drop null term

  x = decode_uint16(packet.body - 2);
  y = decode_uint16(packet.body);

  sc_mouse_coords coords = {x, y};

  return coords;
}

void sc_bytestream_get_event(int fd) {
  sc_bytestream_packet packet = deserialize_packet(fd);
}

// ENCODING/DECODING HELPERS

sc_bytestream_header create_header(uint8_t type) {
  sc_bytestream_header header = {(uint16_t)type, time(NULL)};
  return header;
}

void serialize_packet(int fd, sc_bytestream_packet packet) {
  tpl_node *tn = tpl_map(TPL_STRUCTURE, &packet);
  sc_bytestream_header header = packet.header;
  tpl_pack(tn, 0);
  tpl_dump(tn, TPL_FD, fd);
  tpl_free(tn);
}

sc_bytestream_packet deserialize_packet(int fd) {
  sc_bytestream_packet packet;

  tpl_node *tn = tpl_map(TPL_STRUCTURE, &packet);
  tpl_load(tn, TPL_FD, fd);
  tpl_unpack(tn, 0);
  tpl_free(tn);

  return packet;
}

char * append_uint16(char *output, char *end, uint16_t val) {
  if((output + 2) > end)
    return NULL;

  output[1] = val & 0xff;
  output[0] = val >> 8;
  return output+2;
}

uint16_t decode_uint16(const char *data) {
  unsigned char *c = (unsigned char *) data;
  uint16_t val;
  val = (c[0] << 8) | c[1];
  return val;
}