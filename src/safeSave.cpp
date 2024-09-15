#include "safeSave.h"
#include "safeSaveCore.h"
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

const Variant &SafeSave::operator[](const Variant &p_key) const {
	return content[p_key];
}

Variant &SafeSave::operator[](const Variant &p_key) {
	return content[p_key];
}

Dictionary SafeSave::get_dict() {
	Dictionary d;
	for (const KeyValue<String, Variant> &E : content) {
		d[E.key] = E.value;
	}
	return d;
}

void SafeSave::set_dict(Dictionary dict) {
	Array keys = dict.keys();
	content.clear();
	for (int i = 0; i < keys.size(); i++) {
		content[keys[i]] = dict[keys[i]];
	}
}

Error SafeSave::load(const String &p_path) {
	String absolutePath = ProjectSettings::get_singleton()->globalize_path(p_path);
	PackedByteArray raw_data;
	Error err = safeLoad(raw_data, absolutePath);
	if (err != OK)
		return err;
	deserialize_hashmap(raw_data, content);
	// content = UtilityFunctions::bytes_to_var(raw_data);
	return OK;
}

Error SafeSave::save(const String &p_path) {
	String absolutePath = ProjectSettings::get_singleton()->globalize_path(p_path);
	PackedByteArray raw_data = serialize_hashmap(content);
	// PackedByteArray raw_data = UtilityFunctions::var_to_bytes(res->get_dict());
	return safeSave(raw_data, absolutePath);
}

void SafeSave::set(const String &key, const Variant &value) {
	//if (value.get_type() == Variant::NIL) {
	//	content.erase(key);
	//}
	content[key] = value;
}

bool SafeSave::has(const String &key) const {
	return content.has(key);
}

Variant SafeSave::get(const String &key, const String &p_default = "") const {
	if (!has(key)) {
		return p_default;
	}
	return content[key];
}


void SafeSave::clear() {
	content.clear();
}

// Serialize a HashMap<String, Variant> to a PackedByteArray
PackedByteArray SafeSave::serialize_hashmap(const HashMap<String, Variant> &map) {
	PackedByteArray buffer;

	// Serialize the number of elements (4 bytes)
	uint32_t size = map.size();
	buffer.resize(4);
	memcpy(buffer.ptrw(), &size, 4);

	// Serialize each key-value pair
	for (const KeyValue<String, Variant> &it : map) {
		const String &key = it.key;
		const Variant &value = it.value;

		// Serialize the key size (1 byte for small string size optimization)
		uint8_t key_size = key.length();
		buffer.push_back(key_size);

		// Serialize the key (as UTF-8 bytes)
		PackedByteArray key_bytes;
		key_bytes.resize(key_size);
		memcpy(key_bytes.ptrw(), key.utf8().get_data(), key_size);
		buffer.append_array(key_bytes);

		// Serialize the Variant value using var_to_bytes
		PackedByteArray value_bytes = UtilityFunctions::var_to_bytes(value);
		buffer.append_array(value_bytes); // Append the serialized value
	}

	return buffer;
}

// Deserialize a PackedByteArray into a HashMap<String, Variant>
Error SafeSave::deserialize_hashmap(const PackedByteArray &buffer, HashMap<String, Variant> &r_map) {
	const uint8_t *buf = buffer.ptr();
	int len = buffer.size();

	// Deserialize the number of elements (first 4 bytes)
	if (len < 4) {
		return ERR_INVALID_DATA;
	}

	uint32_t size;
	memcpy(&size, buf, 4);

	buf += 4;
	len -= 4;

	// Deserialize each key-value pair
	for (uint32_t i = 0; i < size; ++i) {
		// Deserialize the key size (1 byte)
		if (len < 1) {
			return ERR_INVALID_DATA;
		}
		uint8_t key_size = *buf++;
		len--;

		// Deserialize the key string (UTF-8)
		if (len < key_size) {
			return ERR_INVALID_DATA;
		}
		String key;
		key.parse_utf8((const char *)buf, key_size);
		buf += key_size;
		len -= key_size;

		// Deserialize the Variant value using bytes_to_var
		PackedByteArray value_bytes;
		value_bytes.resize(len); // Allocate remaining bytes for the value
		memcpy(value_bytes.ptrw(), buf, len);

		Variant value = UtilityFunctions::bytes_to_var(value_bytes);
		buf += value_bytes.size();
		len -= value_bytes.size();

		// Insert into the HashMap
		r_map.insert(key, value);
	}

	return OK;
}

void SafeSave::_bind_methods() {
	ClassDB::bind_method(D_METHOD("save", "path"), &SafeSave::save);
	ClassDB::bind_method(D_METHOD("load", "path"), &SafeSave::load);

	ClassDB::bind_method(D_METHOD("set", "key", "value"), &SafeSave::set);
	ClassDB::bind_method(D_METHOD("set", "key", "value"), &SafeSave::set);
	ClassDB::bind_method(D_METHOD("get", "key", "default"), &SafeSave::get);
	ClassDB::bind_method(D_METHOD("has", "key"), &SafeSave::has);
	ClassDB::bind_method(D_METHOD("clear"), &SafeSave::clear);

	ClassDB::bind_method(D_METHOD("get_dict"), &SafeSave::get_dict);
	ClassDB::bind_method(D_METHOD("set_dict", "dict"), &SafeSave::set_dict);
}
