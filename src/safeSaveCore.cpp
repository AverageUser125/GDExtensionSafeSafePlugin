///////////////////////////////////////////
//do not remove this notice
//(c) Luta Vlad
// 
// safeSave 1.0.0
// 
///////////////////////////////////////////

#include <cstdlib>
#include <cstring>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "safeSaveCore.h"
using namespace godot;

Error readEntireFile(std::vector<char>& data, const char* name)
{
    data.clear();
    std::ifstream f(name, std::ios::binary);

    if(!f.is_open())
    {
        return ERR_FILE_CANT_OPEN;
    }
    f.seekg(0, std::ios_base::end);
    size_t size = f.tellg();
    f.seekg(0, std::ios_base::beg);

    data.resize(size);

    f.read(&data[0], size);

    return OK;
}

Error readEntireFile(void* data, size_t size, const char* name, bool shouldMatchSize, int* bytesRead)
{
    if (bytesRead)
    {
        *bytesRead = 0;
    }

    std::ifstream f(name, std::ios::binary);
    if (!f.is_open())
    {
        return ERR_FILE_CANT_OPEN;
    }
    f.seekg(0, std::ios_base::end);
    size_t readSize = f.tellg();
    f.seekg(0, std::ios_base::beg);

    if (shouldMatchSize)
    {
        if (readSize != size)
        {
            return ERR_PARAMETER_RANGE_ERROR;
        }
        f.read((char*)data, readSize);
        if (bytesRead)
        {
            *bytesRead = readSize;
        }

        return OK;
    }
    
    f.read((char*)data, std::min(size, readSize));

    if (bytesRead)
    {
        *bytesRead = std::min(size, readSize);
    }

    return OK;

}

Error getFileSize(const char *name, size_t &size)
{
    size = 0;

    std::ifstream f(name, std::ios::binary);

    if (!f.is_open())
    {
        return ERR_FILE_CANT_OPEN;
    }
    f.seekg(0, std::ios_base::end);
    size_t readSize = f.tellg();
    f.close();
    size = readSize;

    return OK;
}

using HashType = unsigned long long;

//https://stackoverflow.com/questions/34595/what-is-a-good-hash-function
HashType fnv_hash_1a_64(const void* key, int len)
{
    const unsigned char* p = (const unsigned char*)key;
    HashType h = 0xcbf29ce484222325ULL;

    //for (int i = 0; i < len; i++)
    //{
    //	h = (h ^ p[i]) * 0x100000001b3ULL;
    //}

    if(len >= 4)
    for (int i = 0; i < len-3; i+=4)
    {
        h = (h ^ p[i + 0]) * 0x100000001b3ULL;
        h = (h ^ p[i + 1]) * 0x100000001b3ULL;
        h = (h ^ p[i + 2]) * 0x100000001b3ULL;
        h = (h ^ p[i + 3]) * 0x100000001b3ULL;
    }
    
    for (int i = len - (len%4); i < len; i++)
    {
        h = (h ^ p[i]) * 0x100000001b3ULL;
    }

    return h;
}

Error readEntireFileWithCheckSum(void* data, size_t size, const char* name)
{
    std::ifstream f(name, std::ios::binary);
    if (!f.is_open())
    {
         return ERR_FILE_CANT_OPEN;
 
    }
    f.seekg(0, std::ios_base::end);
    size_t readSize = f.tellg();
    f.seekg(0, std::ios_base::beg);
    
    size_t sizeWithCheckSum = size + sizeof(HashType);

    if (readSize != sizeWithCheckSum)
    {
        return ERR_PARAMETER_RANGE_ERROR;
    }
    f.read((char*)data, size);
    HashType checkSum = 0;
    f.read((char*)&checkSum, sizeof(HashType));

    HashType testCheck = fnv_hash_1a_64(data, size);

    if (testCheck != checkSum)
    {
        return ERR_FILE_CORRUPT;
    }
    
    return OK;
}

Error readEntireFileWithCheckSum(std::vector<char> &data, const char *name)
{
    data.clear();

    std::ifstream f(name, std::ios::binary);
    if (!f.is_open())
    {
        return ERR_FILE_CANT_OPEN;
    }
    f.seekg(0, std::ios_base::end);
    size_t readSize = f.tellg();
    f.seekg(0, std::ios_base::beg);

    if(readSize <= sizeof(HashType))
    {
        return ERR_FILE_CORRUPT;
    }
    data.resize(readSize - sizeof(HashType));
    f.read(&data[0], readSize - sizeof(HashType));

    HashType checkSum = 0;
    f.read((char *)&checkSum, sizeof(HashType));

    auto testCheck = fnv_hash_1a_64(&data[0], data.size());

    if (testCheck != checkSum)
    {
        return ERR_FILE_CORRUPT;
    }
    return OK; 
}

Error readEntireFileWithCheckSum(PackedByteArray& data, const char* name) {
	data.clear();

	std::ifstream f(name, std::ios::binary);
	if (!f.is_open()) {
		return ERR_FILE_CANT_OPEN;
	}
	f.seekg(0, std::ios_base::end);
	size_t readSize = f.tellg();
	f.seekg(0, std::ios_base::beg);

	if (readSize <= sizeof(HashType)) {
		return ERR_FILE_CORRUPT;
	}
	data.resize(readSize - sizeof(HashType));
	f.read((char*)data.ptrw(), readSize - sizeof(HashType));

	HashType checkSum = 0;
	f.read((char*)&checkSum, sizeof(HashType));

	auto testCheck = fnv_hash_1a_64(data.ptr(), data.size());

	if (testCheck != checkSum) {
		return ERR_FILE_CORRUPT;
	}
	return OK;
}

Error writeEntireFileWithCheckSum(const void* data, size_t size, const char* name) {
	std::ofstream f(name, std::ios::binary);

	if (!f.is_open()) {
		return ERR_FILE_CANT_OPEN;
    }
	f.write((char*)data, size);
	HashType testCheck = fnv_hash_1a_64(data, size);
	f.write((char*)&testCheck, sizeof(testCheck));
	return OK;
}

Error writeEntireFile(const std::vector<char>& data, const char* name)
{
    return writeEntireFile(data.data(), data.size(), name);
}

Error writeEntireFile(const void* data, size_t size, const char* name)
{
    std::ofstream f(name, std::ios::binary);

    if (!f.is_open())
    {
        return ERR_FILE_CANT_OPEN;
    }
    f.write((char*)data, size);
    return OK;
}

Error safeSave(const void* data, size_t size, const char* nameWithoutExtension)
{
    std::string file1 = nameWithoutExtension; file1 += "1.bin";
    std::string file2 = nameWithoutExtension; file2 += "2.bin";

    Error err1 = writeEntireFileWithCheckSum((char*)data, size, file1.c_str());

    Error err2 = writeEntireFileWithCheckSum((char*)data, size, file2.c_str());	

    if (err1 != OK && err2 != OK) {
		if (err1 == OK) {
			return err2;
		}
		return err1;
	}
    return OK;
}

Error safeLoad(void* data, size_t size, const char* nameWithoutExtension)
{
    std::string file1 = nameWithoutExtension; file1 += "1.bin";
    std::string file2 = nameWithoutExtension; file2 += "2.bin";

    Error err = readEntireFileWithCheckSum((char*)data, size, file1.c_str());

    if (err == OK)
    {
        return OK;
    }
    
    //load backup
    Error err2 = readEntireFileWithCheckSum((char*)data, size, file2.c_str());

    // return even if err2 is OK, because it's not an error
    return err2;
}

Error safeLoad(PackedByteArray& data, const char *nameWithoutExtension)
{
    data.clear();

    std::string file1 = nameWithoutExtension; file1 += "1.bin";
    std::string file2 = nameWithoutExtension; file2 += "2.bin";

    Error err1 = readEntireFileWithCheckSum(data, file1.c_str());

    if (err1 == OK)
    {
        return OK;
    }
    //load backup
    Error err2 = readEntireFileWithCheckSum(data, file2.c_str());
    return err2;
    
}

Error safeLoad(std::vector<char>& data, const char* nameWithoutExtension) {
	data.clear();

	std::string file1 = nameWithoutExtension;
	file1 += "1.bin";
	std::string file2 = nameWithoutExtension;
	file2 += "2.bin";

	Error err = readEntireFileWithCheckSum(data, file1.c_str());

	if (err == OK) {
		return OK;
	} else {
		//load backup
		Error err2 = readEntireFileWithCheckSum(data, file2.c_str());
		return err2;
	}
}

Error safeLoadBackup(void* data, size_t size, const char* nameWithoutExtension)
{
    std::string file2 = nameWithoutExtension; file2 += "2.bin";

    //load backup
    return readEntireFileWithCheckSum((char*)data, size, file2.c_str());
}
