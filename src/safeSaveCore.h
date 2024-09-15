///////////////////////////////////////////
//do not remove this notice
//(c) Luta Vlad
//
// safeSave 1.0.0
//
///////////////////////////////////////////

#pragma once
#include <fstream>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <unordered_map>
#include <vector>

using namespace godot;

#ifdef _MSC_VER
#pragma warning(disable : 26812)
#endif

Error readEntireFile(std::vector<char> &data, const char *name);

//reades the content of a file (size bytes), if shouldMatchSize is false will read the entire fill untill size bytes are read or the entire file was read
Error readEntireFile(void *data, size_t size, const char *name, bool shouldMatchSize, int *bytesRead = nullptr);

//gets the file size
Error getFileSize(const char *name, size_t &size);

//reades the entire content of the data to a file and uses checkSum
Error readEntireFileWithCheckSum(void *data, size_t size, const char *name);

//reades the entire content of the data to a file and uses checkSum
Error readEntireFileWithCheckSum(std::vector<char> &data, const char *name);

//reades the entire content of the data to a file and uses checkSum
Error readEntireFileWithCheckSum(PackedByteArray &data, const char *name);

//writes the entire content of the data to a file and uses checkSum
Error writeEntireFileWithCheckSum(const void *data, size_t size, const char *name);

//writes the entire content of the data to a file
Error writeEntireFile(const std::vector<char> &data, const char *name);

//writes the entire content of the data to a file
Error writeEntireFile(const void *data, size_t size, const char *name);

//saved the data with a check sum and a backup
Error safeSave(const void *data, size_t size, const char *nameWithoutExtension);

//loads the data that was saved using safeSave
Error safeLoad(void *data, size_t size, const char *nameWithoutExtension);

//loads the data that was saved using safeSave and stored as a SafeSafeKeyValueData structure
Error safeLoad(std::vector<char> &data, const char *nameWithoutExtension);

//loads the data that was saved using safeSave and stored as a SafeSafeKeyValueData structure
Error safeLoad(PackedByteArray &data, const char *nameWithoutExtension);

//same as safeLoad but only loads the backup file.
Error safeLoadBackup(void *data, size_t size, const char *nameWithoutExtension);
