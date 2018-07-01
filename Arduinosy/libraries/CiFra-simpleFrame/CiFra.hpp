#pragma once

#include <stdint.h>

/*
CItrus FRAme - multipurpose simple frame with fixed length
Start[2B]| Addr[2B] |MsgId[1B]|    Type[1B]      | Payload[2B] |Checksum[2B]|End[2B]
   HI    |uniqueAddr|uniqueId |status/command/ack|value/command|   CRC      |  BI
*/

namespace cifra
{

class CiFra
{
public:
  typedef uint16_t Address;
  typedef uint8_t  Id;
  typedef uint16_t Payload;
  typedef uint16_t Checksum;

  enum class FrameKey : uint16_t
  {
    HI = 0x4849, // HI - frame begin
    BI = 0x4249  // BI - frame end
  };

  enum class Type : uint8_t
  {
    Status = 1,
    Command = 2,
    Ack = 3
  };

  const Address broadcstAddr = 0xFFFF; // broadcast address

public:
  CiFra(const Address addr, const Type typ, const Payload pyload);
  ~CiFra();

  uint8_t* getFrame();

private:
  Id getId();
  Checksum calculateChecksum();
  uint16_t fletcher16();
  uint16_t fletcher16( uint8_t const *data, uint16_t len );

  const Address  address;
  const Id       id;
  const Type     type;
  const Payload  payload;
  Checksum checksum;
};

} //namespace cifra
