extends Node
const filepath: String = "user://test"
var res: SafeSave = SafeSave.new()

func _ready():
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	pass


func _on_save_button_pressed():
	var data = JSON.parse_string($InputText.text)
	if data == null:
		$OutputText.text = "ERROR: enter a json dictionary"
		return
	$OutputText.text = ""
	
	res.set_dict(data)
	var err = res.save(filepath)
	if err == OK:
		$OutputText.text = "Saving success!"
	else:
		$OutputText.text ="ERROR:" + error_string(err)
		

func packed_string_array_join(packed_array: PackedStringArray) -> String:
	var result := ""
	for i in range(packed_array.size()):
		var string_value := String(packed_array[i])
		result += string_value + "\n"
	return result

func _on_load_button_pressed():
	var err = res.load(filepath)
	if err != OK:
		$OutputText.text = "ERROR: " + error_string(err)
		return
	var data = res.get_dict()

	$OutputText.text = var_to_string_fancy(data)


static func var_to_string_fancy(data) -> String:
	if typeof(data) == TYPE_STRING:
		return data
	elif typeof(data) == TYPE_DICTIONARY:
		return JSON.stringify(data)
	else:
		return var_to_str(data)
