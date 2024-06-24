#include "IRrecv.h"
#include "IRsend.h"
#include "IRutils.h"

// WARNING: This probably isn't directly usable. It's a guide only.

// See https://github.com/crankyoldgit/IRremoteESP8266/wiki/Adding-support-for-a-new-IR-protocol
// for details of how to include this in the library.
const uint16_t kTestExampleHdrMark = 4958;
const uint16_t kTestExampleBitMark = 468;
const uint16_t kTestExampleHdrSpace = 5034;
const uint16_t kTestExampleOneSpace = 1550;
const uint16_t kTestExampleZeroSpace = 554;
const uint16_t kTestExampleFreq = 38000;  // Hz. (Guessing the most common frequency.)
const uint16_t kTestExampleBits = 104;  // Move to IRremoteESP8266.h
const uint16_t kTestExampleStateLength = 13;  // Move to IRremoteESP8266.h
const uint16_t kTestExampleOverhead = 3;
// DANGER: More than 64 bits detected. A uint64_t for 'data' won't work!

#if SEND_TESTEXAMPLE
// Function should be safe up to 64 bits.
/// Send a TestExample formatted message.
/// Status: ALPHA / Untested.
/// @param[in] data containing the IR command.
/// @param[in] nbits Nr. of bits to send. usually kTestExampleBits
/// @param[in] repeat Nr. of times the message is to be repeated.
void IRsend::sendTestExample(const uint64_t data, const uint16_t nbits, const uint16_t repeat) {
  enableIROut(kTestExampleFreq);
  for (uint16_t r = 0; r <= repeat; r++) {
    uint64_t send_data = data;
    // Header
    mark(kTestExampleHdrMark);
    space(kTestExampleHdrSpace);
    // Data Section #1
    // e.g. data = 0xD54ED72FFA98FFFBFF7F66FDDC, nbits = 104
    sendData(kTestExampleBitMark, kTestExampleOneSpace, kTestExampleBitMark, kTestExampleZeroSpace, send_data, 104, true);
    send_data >>= 104;
    // Header
    mark(kTestExampleHdrMark);
    space(kDefaultMessageGap);  // A 100% made up guess of the gap between messages.
  }
}
#endif  // SEND_TESTEXAMPLE

#if SEND_TESTEXAMPLE
// Alternative >64bit function to send TESTEXAMPLE messages
// Function should be safe over 64 bits.
/// Send a TestExample formatted message.
/// Status: ALPHA / Untested.
/// @param[in] data An array of bytes containing the IR command.
///                 It is assumed to be in MSB order for this code.
/// e.g.
/// @code
///   uint8_t data[kTestExampleStateLength] = {0xD5, 0x4E, 0xD7, 0x2F, 0xFA, 0x98, 0xFF, 0xFB, 0xFF, 0x7F, 0x66, 0xFD, 0xDC};
/// @endcode
/// @param[in] nbytes Nr. of bytes of data in the array. (>=kTestExampleStateLength)
/// @param[in] repeat Nr. of times the message is to be repeated.
void IRsend::sendTestExample(const uint8_t data[], const uint16_t nbytes, const uint16_t repeat) {
  for (uint16_t r = 0; r <= repeat; r++) {
    uint16_t pos = 0;
    // Data Section #2
    // e.g.
    //   bits = 104; bytes = 13;
    //   *(data + pos) = {0xD5, 0x4E, 0xD7, 0x2F, 0xFA, 0x98, 0xFF, 0xFB, 0xFF, 0x7F, 0x66, 0xFD, 0xDC};
    sendGeneric(kTestExampleHdrMark, kTestExampleHdrSpace,
                kTestExampleBitMark, kTestExampleOneSpace,
                kTestExampleBitMark, kTestExampleZeroSpace,
                kTestExampleHdrMark, kDefaultMessageGap,
                data + pos, 13,  // Bytes
                kTestExampleFreq, true, kNoRepeat, kDutyDefault);
    pos += 13;  // Adjust by how many bytes of data we sent
  }
}
#endif  // SEND_TESTEXAMPLE

// DANGER: More than 64 bits detected. A uint64_t for 'data' won't work!
#if DECODE_TESTEXAMPLE
// Function should be safe up to 64 bits.
/// Decode the supplied TestExample message.
/// Status: ALPHA / Untested.
/// @param[in,out] results Ptr to the data to decode & where to store the decode
/// @param[in] offset The starting index to use when attempting to decode the
///   raw data. Typically/Defaults to kStartOffset.
/// @param[in] nbits The number of data bits to expect.
/// @param[in] strict Flag indicating if we should perform strict matching.
/// @return A boolean. True if it can decode it, false if it can't.
bool IRrecv::decodeTestExample(decode_results *results, uint16_t offset, const uint16_t nbits, const bool strict) {
  if (results->rawlen < 2 * nbits + kTestExampleOverhead - offset)
    return false;  // Too short a message to match.
  if (strict && nbits != kTestExampleBits)
    return false;

  uint64_t data = 0;
  match_result_t data_result;

  // Header
  if (!matchMark(results->rawbuf[offset++], kTestExampleHdrMark))
    return false;
  if (!matchSpace(results->rawbuf[offset++], kTestExampleHdrSpace))
    return false;

  // Data Section #1
  // e.g. data_result.data = 0xD54ED72FFA98FFFBFF7F66FDDC, nbits = 104
  data_result = matchData(&(results->rawbuf[offset]), 104,
                          kTestExampleBitMark, kTestExampleOneSpace,
                          kTestExampleBitMark, kTestExampleZeroSpace);
  offset += data_result.used;
  if (data_result.success == false) return false;  // Fail
  data <<= 104;  // Make room for the new bits of data.
  data |= data_result.data;

  // Header
  if (!matchMark(results->rawbuf[offset++], kTestExampleHdrMark))
    return false;

  // Success
  results->decode_type = decode_type_t::TESTEXAMPLE;
  results->bits = nbits;
  results->value = data;
  results->command = 0;
  results->address = 0;
  return true;
}
#endif  // DECODE_TESTEXAMPLE

#if DECODE_TESTEXAMPLE
// Function should be safe over 64 bits.
/// Decode the supplied TestExample message.
/// Status: ALPHA / Untested.
/// @param[in,out] results Ptr to the data to decode & where to store the decode
/// @param[in] offset The starting index to use when attempting to decode the
///   raw data. Typically/Defaults to kStartOffset.
/// @param[in] nbits The number of data bits to expect.
/// @param[in] strict Flag indicating if we should perform strict matching.
/// @return A boolean. True if it can decode it, false if it can't.
bool IRrecv::decodeTestExample(decode_results *results, uint16_t offset, const uint16_t nbits, const bool strict) {
  if (results->rawlen < 2 * nbits + kTestExampleOverhead - offset)
    return false;  // Too short a message to match.
  if (strict && nbits != kTestExampleBits)
    return false;

  uint16_t pos = 0;
  uint16_t used = 0;

  // Data Section #2
  // e.g.
  //   bits = 104; bytes = 13;
  //   *(results->state + pos) = {0xD5, 0x4E, 0xD7, 0x2F, 0xFA, 0x98, 0xFF, 0xFB, 0xFF, 0x7F, 0x66, 0xFD, 0xDC};
  used = matchGeneric(results->rawbuf + offset, results->state + pos,
                      results->rawlen - offset, 104,
                      kTestExampleHdrMark, kTestExampleHdrSpace,
                      kTestExampleBitMark, kTestExampleOneSpace,
                      kTestExampleBitMark, kTestExampleZeroSpace,
                      kTestExampleHdrMark, kDefaultMessageGap, true);
  if (used == 0) return false;  // We failed to find any data.
  offset += used;  // Adjust for how much of the message we read.
  pos += 13;  // Adjust by how many bytes of data we read

  // Success
  results->decode_type = decode_type_t::TESTEXAMPLE;
  results->bits = nbits;
  return true;
}
#endif  // DECODE_TESTEXAMPLE