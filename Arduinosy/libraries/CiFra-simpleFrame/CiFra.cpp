#include <CiFra.hpp>


namespace cifra
{

CiFra::CiFra(const Address addr, const Type typ, const Payload pyload)
  : address(addr)
  , id(getId())
  , type(typ)
  , payload(pyload)
{
  checksum = calculateChecksum();
}

CiFra::~CiFra()
{}

uint8_t* CiFra::getFrame()
{
  static uint8_t data[12];
  data[0] = (static_cast<uint16_t>(FrameKey::HI) & 0xF);
  data[1] = ((static_cast<uint16_t>(FrameKey::HI) >> 8) & 0xF);

  data[2] = (address & 0xF);
  data[3] = ((address >> 8) & 0xF);

  data[4] = (id);
  data[5] = (static_cast<uint8_t>(type));

  data[6] = (payload & 0xF);
  data[7] = ((payload >> 8) & 0xF);

  data[8] = (checksum & 0xF);
  data[9] = ((checksum >> 8) & 0xF);

  data[10] = (static_cast<uint16_t>(FrameKey::BI) & 0xF);
  data[11] = ((static_cast<uint16_t>(FrameKey::BI) >> 8) & 0xF);

  return data;
}

CiFra::Id CiFra::getId()
{
  static Id id = 1;
  if (++id == 0)
  {
    id = 1;
  }
  return id;
}

CiFra::Checksum CiFra::calculateChecksum()
{
  return fletcher16();
}

uint16_t CiFra::fletcher16()
{
  uint16_t sum1 = 0xFF, sum2 = 0xFF;

  sum1 += address & 0xF;
  sum2 += sum1;
  sum1 += (address >> 8) & 0xF;
  sum2 += sum1;
  sum1 += id;
  sum2 += sum1;
  sum1 += static_cast<uint16_t>(type);
  sum2 += sum1;
  sum1 += payload & 0xF;
  sum2 += sum1;
  sum1 += (payload >> 8) & 0xF;
  sum2 += sum1;

  /* Second reduction step to reduce sums to 16 bits */
  sum1 = (sum1 & 0xFF) + (sum1 >> 8);
  sum2 = (sum2 & 0xFF) + (sum2 >> 8);

  return sum2 << 8 | sum1;
}

uint16_t CiFra::fletcher16( uint8_t const *data, uint16_t len )
{
  uint16_t sum1 = 0xFF, sum2 = 0xFF;

  while (len)
  {
    unsigned tlen = len > 359 ? 359 : len;
    len -= tlen;

    do
    {
      sum1 += *data++;
      sum2 += sum1;
    } while (--tlen);

    sum1 = (sum1 & 0xFF) + (sum1 >> 8);
    sum2 = (sum2 & 0xFF) + (sum2 >> 8);
  }

  /* Second reduction step to reduce sums to 16 bits */
  sum1 = (sum1 & 0xFF) + (sum1 >> 8);
  sum2 = (sum2 & 0xFF) + (sum2 >> 8);

  return sum2 << 8 | sum1;
}

} // namespace cifra
