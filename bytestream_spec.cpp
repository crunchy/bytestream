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

// start and stop both have null bodys
TEST_F(BytestreamTest, EncodeStart) {
  sc_bytestream_packet packet = sc_bytestream_put_start(tmp);

  EXPECT_EQ(START, packet.header.type);
  EXPECT_EQ(NULL, packet.body.addr);

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

