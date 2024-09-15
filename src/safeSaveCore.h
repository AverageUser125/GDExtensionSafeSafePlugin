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
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/string.hpp>
using namespace godot;

#ifdef _MSC_VER
#pragma warning(disable : 26812)
#endif


//reades the entire content of the data to a file and uses checkSum
Error readEntireFileWithCheckSum(void *data, size_t size, String name);

//reades the entire content of the data to a file and uses checkSum
Error readEntireFileWithCheckSum(PackedByteArray &data, String name);

//writes the entire content of the data to a file and uses checkSum
Error writeEntireFileWithCheckSum(const void *data, size_t size, String name);

Error safeSave(const PackedByteArray &data, String nameWithoutExtension);

//saved the data with a check sum and a backup
Error safeSave(const void *data, size_t size, String nameWithoutExtension);

//loads the data that was saved using safeSave
Error safeLoad(void *data, size_t size, String nameWithoutExtension);

//loads the data that was saved using safeSave and stored as a SafeSafeKeyValueData structure
Error safeLoad(PackedByteArray &data, String nameWithoutExtension);
