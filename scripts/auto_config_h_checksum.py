# This script adds the last four digits of the SHA256 hash of config.h as a preprocessor flag,
# which is used to invalidate the LoRa session stored in FRAM in case of a configuration change

Import("env")
import hashlib

def get_config_checksum_flag():
  path_config_h = r"src/config.h"
  print("config.h: ", end="")
  try:
    with open(path_config_h, "rb") as f:
      digest = hashlib.file_digest(f, "sha256").hexdigest()
    build_flag = "-DCONFIG_H_CHECKSUM=0x" + digest[-4:]
    print("checksum is 0x", digest[-4:], sep="")
  except FileNotFoundError:
    build_flag = ""
    print("file not found!")
  return(build_flag)

env.Append(
  BUILD_FLAGS=[get_config_checksum_flag()]
)