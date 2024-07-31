// Payload decoder for TTN v3 & Chirpstack v4, JS code based on ATTNode v3 example (https://www.attno.de/)

function bytesToInt16(bytes, start) {
  var out = ((bytes[start]) | (bytes[start + 1] << 8));
  var sign = bytes[start + 1] & (1 << 7);
  if (sign) {
    out = 0xFFFF0000 | out;
  }
  return out;
}

function bytesToUInt16(bytes, start) {
  return ((bytes[start]) | (bytes[start + 1] << 8));
}

function bytesToInt32(bytes, start) {
  return ((bytes[start]) | (bytes[start + 1] << 8) | (bytes[start + 2] << 16) | (bytes[start + 3] << 24));
}

// Decode uplink function.
//
// Input is an object with the following fields:
// - bytes = Byte array containing the uplink payload, e.g. [255, 230, 255, 0]
// - fPort = Uplink fPort.
// - variables = Object containing the configured device variables.
//
// Output must be an object with the following fields:
// - data = Object representing the decoded payload.
function decodeUplink(input) {
  var decoded = {};
  var i = 0;

  if (input.bytes.length >= 4) {
    // Batt (uint16_t)
    decoded.battery = bytesToUInt16(input.bytes, i) / 1000;
    i += 2;

    // Count (uint16_t)
    decoded.count = bytesToUInt16(input.bytes, i);
    decoded.mm = decoded.count * 0.45;
    i += 2;
  }

  if (input.bytes.length >= 12) {
    // Temperature (int32_t)
    decoded.temperature = bytesToInt32(input.bytes, i) / 100;
    i += 4;

    // Atmospheric Pressure (int32_t)
    decoded.pressure = bytesToInt32(input.bytes, i) / 100;
    i += 4;
  }

  if (input.bytes.length >= 14) {
    // Humidity (int32_t)
    decoded.humidity = bytesToInt32(input.bytes, i) / 1000;
    i += 4;
  }

  return {
    data: decoded,
    warnings: [],
    errors: []
  };
}

// Encode downlink function.
//
// Input is an object with the following fields:
// - data = Object representing the payload that must be encoded.
// - variables = Object containing the configured device variables.
//
// Output must be an object with the following fields:
// - bytes = Byte array containing the downlink payload.
function encodeDownlink(input) {
  return {
    bytes: []
  };
}
