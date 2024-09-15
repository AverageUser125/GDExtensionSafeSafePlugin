///////////////////////////////////////////
//do not remove this notice
//(c) Luta Vlad
//
// safeSave 1.0.0
//
///////////////////////////////////////////

#include "safeSaveCore.h"
#include <godot_cpp/classes/file_access.hpp>
using namespace godot;

constexpr auto SAVE_FILE_EXTENSION = ".bin";

using HashType = unsigned long long;

//https://stackoverflow.com/questions/34595/what-is-a-good-hash-function
HashType fnv_hash_1a_64(const void *key, int len) {
	const unsigned char *p = (const unsigned char *)key;
	HashType h = 0xcbf29ce484222325ULL;

	//for (int i = 0; i < len; i++)
	//{
	//	h = (h ^ p[i]) * 0x100000001b3ULL;
	//}

	if (len >= 4)
		for (int i = 0; i < len - 3; i += 4) {
			h = (h ^ p[i + 0]) * 0x100000001b3ULL;
			h = (h ^ p[i + 1]) * 0x100000001b3ULL;
			h = (h ^ p[i + 2]) * 0x100000001b3ULL;
			h = (h ^ p[i + 3]) * 0x100000001b3ULL;
		}

	for (int i = len - (len % 4); i < len; i++) {
		h = (h ^ p[i]) * 0x100000001b3ULL;
	}

	return h;
}

Error readEntireFileWithCheckSum(void *data, size_t size,String name) {
	Ref<FileAccess> f = FileAccess::open(name, FileAccess::READ);

	if (f.is_null() || !f->is_open()) {
		return ERR_FILE_CANT_OPEN;
	}

	size_t dataSize = f->get_length() - sizeof(HashType);

	f->get_buffer((uint8_t*)data, dataSize);

	// f->seek(readSize -sizeof(HashType));
	HashType checkSum = 0;
	f->get_buffer((uint8_t *)&checkSum, sizeof(HashType));

	HashType testCheck = fnv_hash_1a_64(data, size);

	if (testCheck != checkSum) {
		return ERR_FILE_CORRUPT;
	}
	return OK;
}

Error readEntireFileWithCheckSum(PackedByteArray &data, String name) {
	data.clear();

	Ref<FileAccess> f = FileAccess::open(name, FileAccess::READ);

	if (f.is_null() || !f->is_open()) {
		return ERR_FILE_CANT_OPEN;
	}

	size_t dataSize = f->get_length() - sizeof(HashType);
	
	data.resize(dataSize);
	f->get_buffer(data.ptrw(), dataSize);

	// f->seek(readSize -sizeof(HashType));
	HashType checkSum = 0;
	f->get_buffer((uint8_t *)&checkSum, sizeof(HashType));

	HashType testCheck = fnv_hash_1a_64(data.ptr(), data.size());

	if (testCheck != checkSum) {
		return ERR_FILE_CORRUPT;
	}
	return OK;
}

Error writeEntireFileWithCheckSum(const void *data, size_t size, String name) {
	Ref<FileAccess> f = FileAccess::open(name, FileAccess::WRITE);

	// std::ofstream f(name, std::ios::binary);

	if (f.is_null() || !f->is_open()) {
		return ERR_FILE_CANT_OPEN;
	}
	f->store_buffer((const uint8_t*)data, size);
	HashType testCheck = fnv_hash_1a_64(data, size);
	f->store_buffer((const uint8_t *)&testCheck, sizeof(HashType));
	return OK;
}

Error safeSave(const PackedByteArray& data, String nameWithoutExtension) {
	return safeSave(data.ptr(), data.size(), nameWithoutExtension);
}

Error safeSave(const void *data, size_t size, String nameWithoutExtension) {
	String file1 = nameWithoutExtension + "1" + SAVE_FILE_EXTENSION;
	String file2 = nameWithoutExtension + "2" + SAVE_FILE_EXTENSION;

	Error err1 = writeEntireFileWithCheckSum(data, size, file1);

	Error err2 = writeEntireFileWithCheckSum(data, size, file2);

	if (err1 != OK && err2 != OK) {
		if (err1 == OK) {
			return err2;
		}
		return err1;
	}
	return OK;
}

Error safeLoad(void *data, size_t size, String nameWithoutExtension) {
	String file1 = nameWithoutExtension + "1" + SAVE_FILE_EXTENSION;
	String file2 = nameWithoutExtension + "2" + SAVE_FILE_EXTENSION;

	Error err = readEntireFileWithCheckSum((char *)data, size, file1);

	if (err == OK) {
		return OK;
	}

	//load backup
	Error err2 = readEntireFileWithCheckSum((char *)data, size, file2);

	// return even if err2 is OK, because it's not an error
	return err2;
}

Error safeLoad(PackedByteArray &data, String nameWithoutExtension) {
	data.clear();

	String file1 = nameWithoutExtension + "1" + SAVE_FILE_EXTENSION;
	String file2 = nameWithoutExtension + "2" + SAVE_FILE_EXTENSION;

	Error err1 = readEntireFileWithCheckSum(data, file1);

	if (err1 == OK) {
		return OK;
	}
	//load backup
	Error err2 = readEntireFileWithCheckSum(data, file2);
	return err2;
}