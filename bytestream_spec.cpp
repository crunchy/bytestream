// Tests

#include "gtest/gtest.h"
#include "bytestream.h"

class BytestreamTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    tmp = open("test.tpl", O_RDWR | O_CREAT | O_TRUNC, 0666);
  }

  virtual void TearDown() {
    close(tmp);
  }

  int tmp;
};

// simple header creation helper
TEST_F(BytestreamTest, CreateHeader) {
  sc_bytestream_header h = create_header(MOUSE);
  sc_time timeNow = time(NULL);

  EXPECT_EQ(MOUSE, h.type);
  EXPECT_EQ(timeNow, h.timestamp);
};

TEST_F(BytestreamTest, PackingHelpers) {
  char buffer[35], *enc = buffer, *end = buffer + sizeof(buffer);
  uint16_t one, two, three;

  enc = append_uint16(enc, end, 42);
  enc = append_uint16(enc, end, 57);
  append_uint16(enc, end, 29);

  one = decode_uint16(enc - 4);
  two = decode_uint16(enc - 2);
  three = decode_uint16(enc);

  EXPECT_EQ(42, one);
  EXPECT_EQ(57, two);
  EXPECT_EQ(29, three);
};

// start and stop both have null bodys
TEST_F(BytestreamTest, EncodeStart) {
  sc_bytestream_packet packet = sc_bytestream_put_start(tmp);

  EXPECT_EQ(START, packet.header.type);
  EXPECT_EQ(NULL, packet.body);

  lseek(tmp, 0, 0); // rewind tempfile
  sc_bytestream_packet packet_read = deserialize_packet(tmp);

  EXPECT_EQ(packet.header.type, packet_read.header.type);
};

TEST_F(BytestreamTest, EncodeStop) {
  sc_bytestream_packet packet = sc_bytestream_put_stop(tmp);

  lseek(tmp, 0, 0); // rewind tempfile
  sc_bytestream_packet packet_read = deserialize_packet(tmp);

  EXPECT_EQ(packet.header.type, packet_read.header.type);
};

TEST_F(BytestreamTest, EncodeMouseData) {
  sc_mouse_coords coords = {440, 99};

  sc_bytestream_packet packet = sc_bytestream_put_mouse_data(tmp, coords);

  lseek(tmp, 0, 0); // rewind tempfile
  sc_mouse_coords decode_coords = sc_bytestream_get_mouse_data(tmp);

  EXPECT_EQ(coords.x, decode_coords.x);
  EXPECT_EQ(coords.y, decode_coords.y);
};

