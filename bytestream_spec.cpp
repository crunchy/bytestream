// Tests

#include "gtest/gtest.h"
#include "bytestream.h"

class BytestreamTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    tmp = fopen("test.tpl", "w+");
  }

  virtual void TearDown() {
    fclose(tmp);
  }

  FILE *tmp;
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

  fseek(tmp, 0, 0); // rewind tempfile
  sc_bytestream_packet packet_read = deserialize_packet(tmp);

  EXPECT_EQ(packet.header.type, packet_read.header.type);
};

TEST_F(BytestreamTest, EncodeStop) {
  sc_bytestream_packet packet = sc_bytestream_put_stop(tmp);

  fseek(tmp, 0, 0); // rewind tempfile
  sc_bytestream_packet packet_read = deserialize_packet(tmp);

  EXPECT_EQ(packet.header.type, packet_read.header.type);
};

TEST_F(BytestreamTest, EncodeMouseData) {
  sc_mouse_coords coords = {440, 99};

  sc_bytestream_packet packet = sc_bytestream_put_mouse_data(tmp, coords);

  fseek(tmp, 0, 0); // rewind tempfile
  sc_mouse_coords decode_coords = sc_bytestream_get_mouse_data(tmp);

  EXPECT_EQ(coords.x, decode_coords.x);
  EXPECT_EQ(coords.y, decode_coords.y);
};

TEST_F(BytestreamTest, EncodeFrameData) {
  int frameData = 0x499602D2;
  uint8_t *framePtr = (uint8_t *) &frameData;
  int frameSize = sizeof(frameData);

  sc_frame frame = {framePtr, frameSize};

  sc_bytestream_packet packet = sc_bytestream_put_frame(tmp, frame);

  fseek(tmp, 0, 0); // rewind tempfile
  sc_frame decode_frame = sc_bytestream_get_frame(tmp);
  int reconstruct;
  memcpy(&reconstruct, decode_frame.framePtr, decode_frame.size);

  EXPECT_EQ(frameData, reconstruct);
  EXPECT_EQ(frameSize, decode_frame.size);
};

TEST_F(BytestreamTest, GetHeader) {
  sc_bytestream_packet packet = sc_bytestream_put_stop(tmp);

  fseek(tmp, 0, 0); // rewind tempfile
  sc_bytestream_header header = sc_bytestream_get_event_header(tmp);

  EXPECT_EQ(packet.header.type, header.type);
};

TEST_F(BytestreamTest, GetRawPacket) {
  sc_bytestream_packet packet = sc_bytestream_put_start(tmp);

  fseek(tmp, 0, 0); // rewind tempfile
  sc_bytestream_packet decode_packet = sc_bytestream_get_event(tmp);

  EXPECT_EQ(packet.body.sz, decode_packet.body.sz);
  EXPECT_EQ(packet.header.type, decode_packet.header.type);
};

TEST_F(BytestreamTest, NoDataHandledSafely) {
  sc_bytestream_packet decode_packet = sc_bytestream_get_event(tmp);

  EXPECT_EQ(NO_DATA, decode_packet.header.type);
};

TEST_F(BytestreamTest, MultiplePackets) {
  int frameData = 0x499602D2;
  uint8_t *framePtr = (uint8_t *) &frameData;
  int frameSize = sizeof(frameData);
  sc_frame frame = {framePtr, frameSize};
  sc_mouse_coords coords = {440, 99};

  sc_bytestream_put_start(tmp);
  sc_bytestream_put_frame(tmp, frame);
  sc_bytestream_put_mouse_data(tmp, coords);
  sc_bytestream_put_stop(tmp);

  fseek(tmp, 0, 0); // rewind tempfile

  sc_bytestream_packet decode_packet = sc_bytestream_get_event(tmp);
  EXPECT_EQ(START, decode_packet.header.type);

  decode_packet = sc_bytestream_get_event(tmp);
  EXPECT_EQ(VIDEO, decode_packet.header.type);

  decode_packet = sc_bytestream_get_event(tmp);
  EXPECT_EQ(MOUSE, decode_packet.header.type);

  decode_packet = sc_bytestream_get_event(tmp);
  EXPECT_EQ(STOP, decode_packet.header.type);
};
