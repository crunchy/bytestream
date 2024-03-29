// bytestream.c
//
// Author:  Luke van der Hoeven
// Date:    7/5/12

#include <time.h>
#include "bytestream.h"

sc_bytestream_header create_header(uint8_t type) {
  return create_header_with_time(type, time(NULL) * SC_TimeBase);
}

sc_bytestream_header create_header_with_time(uint8_t type, sc_time timestamp) {
  sc_bytestream_header header = {(uint16_t)type, timestamp};
  return header;
}

tpl_bin create_body(void *addr, int sz) {
  tpl_bin body;

  body.addr = addr;
  body.sz = sz;

  return body;
}

sc_bytestream_packet sc_bytestream_put_start(int fd) {
  sc_bytestream_packet packet = {create_header(START), create_body(NULL, 0)};

  serialize_packet(fd, packet);
  return packet;
}

sc_bytestream_packet sc_bytestream_put_stop(int fd) {
  sc_bytestream_packet packet = {create_header(STOP), create_body(NULL, 0)};

  serialize_packet(fd, packet);
  return packet;
}

sc_bytestream_packet sc_bytestream_put_mouse_data(int fd, sc_mouse_coords coords) {
  sc_bytestream_packet packet = {create_header(MOUSE), create_body(&coords, sizeof(coords))};
  serialize_packet(fd, packet);

  return packet;
}

sc_mouse_coords sc_bytestream_get_mouse_data(int fd) {
  sc_bytestream_packet packet = deserialize_packet(fd);

  return parse_mouse_coords(packet);
}

sc_bytestream_packet sc_bytestream_put_frame(int fd, sc_frame frame, sc_time timestamp) {
  sc_bytestream_packet packet = {create_header_with_time(VIDEO, timestamp), create_body(frame.framePtr, frame.size)};
  serialize_packet(fd, packet);

  return packet;
}

sc_frame sc_bytestream_get_frame(int fd) {
  sc_bytestream_packet packet = deserialize_packet(fd);

  return parse_frame(packet);
}

sc_bytestream_packet sc_bytestream_get_event(int fd) {
  sc_bytestream_packet packet = deserialize_packet(fd);
  return packet;
}

sc_bytestream_header sc_bytestream_get_event_header(int fd) {
  sc_bytestream_packet packet = sc_bytestream_get_event(fd);
  return packet.header;
}

// ENCODING/DECODING HELPERS

sc_mouse_coords parse_mouse_coords(sc_bytestream_packet packet) {
  void *data = packet.body.addr;

  sc_mouse_coords coords = *((sc_mouse_coords *) data);
  // free(data);

  return coords;
}

sc_frame parse_frame(sc_bytestream_packet packet) {
  void *data = packet.body.addr;

  sc_frame frame = {(uint8_t *) data, packet.body.sz};
  // free(data);

  return frame;
}

void serialize_packet(int fd, sc_bytestream_packet packet) {
  tpl_node *tn = tpl_map(TPL_STRUCTURE, &packet.header, &packet.body);
  tpl_pack(tn, 0);
  tpl_dump(tn, TPL_FD, fd);
  tpl_free(tn);
}

sc_bytestream_packet deserialize_packet(int fd) {
  sc_bytestream_packet packet;
  sc_bytestream_header header;
  tpl_bin data = {NULL, 0};

  tpl_node *tn = tpl_map(TPL_STRUCTURE, &header, &data);
  int loaded = tpl_load(tn, TPL_FD, fd);

  if( loaded == -1 ) {
    packet.header = create_header(NO_DATA);
  } else {
    tpl_unpack(tn, 0);
    packet.header = header;
  }

  packet.body = data;

  tpl_free(tn);
  if(data.addr != NULL) {
    free(data.addr);
  }

  return packet;
}
