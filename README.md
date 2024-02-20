# Image3

A small, work-in-progress library dedicated to parsing Apple's IMG3 file format. Image3 is a file format used by Apple for 32-bit iOS device firmware files, having been superseded by the Image4 file format for 64-bit devices.

Image3 files are made up of a header, followed by a series of tagged data blocks. The header contains information about the file, such as the size of the file, the combined size of the tagged data blocks, and the size of the signed data. Each tagged data block is different - some contain data, some contain metadata, and some contain signatures. For example, the `DATA` tag contains the actual data, the `ECID` tag contains the device's ECID, and the `SHSH` tag contains the signed hash.

To build the library, simply run `make` in the root directory. This will compile the `image3_cli` tool, which can be used to parse Image3 files. In the future, the library will be made available as a static library, and as a dynamic library. It will also gain support for modifying Image3 files.