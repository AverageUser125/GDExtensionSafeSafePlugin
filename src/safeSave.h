#ifndef  SAFE_SAVE_H
#define SAFE_SAVE_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

class SafeSave : public Object {
	GDCLASS(SafeSave, Object)
public:
	Error load(const String &p_path);
	Error save(const String &p_path);

	const Variant *getptr(const String &key, const String &p_default) const;
	Variant get(const String &key, const String &p_default) const;
	void set(const String &key, const Variant &value);
	bool has(const String &key) const;
	void clear();

	Dictionary get_dict();
	void set_dict(Dictionary dict);

	const Variant &operator[](const Variant &p_key) const;
	Variant &operator[](const Variant &p_key);

	Error _deserialize(const PackedByteArray &buffer);
	PackedByteArray _serialize() const;

protected:
	static void _bind_methods();

private:
	HashMap<String, Variant> content;

	static Error deserialize(const PackedByteArray &buffer, HashMap<String, Variant> &r_map);
	static PackedByteArray serialize(const HashMap<String, Variant> &map);
};
#endif // ! SAFE_SAVE_H
