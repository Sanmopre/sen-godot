class_name SenCamera extends Camera3D

var globe_node: CesiumGeoreference
var target: Node3D = null

@export var target_path := "georeference/sen_godot_ig_result_A400M_GAF_1/yaw_pivot/pitch_pivot/roll_pivot/model"
@export var offset := Vector3(0, 0, 0)
@export var tilesets : Array[Cesium3DTileset]


func _ready() -> void:
	globe_node = get_tree().current_scene.get_node("georeference") as CesiumGeoreference
	if globe_node == null:
		push_error("CesiumGeoreference not found.")
		return

	far = 149597870700.0 + 3 * 1392700000.0

func _process(delta):
	if target == null or !is_instance_valid(target):
		target = get_tree().current_scene.get_node_or_null(target_path)

	if target:
		_update_tilesets()

		var t = target.global_transform
		var basis = t.basis.orthonormalized()

		if basis.determinant() < 0.0:
			basis.x = -basis.x

		var world_offset = basis * offset
		var final_pos = t.origin + world_offset
		global_transform = Transform3D(basis, final_pos)

func _update_tilesets() -> void:
	var camera_xform := self.globe_node.get_tx_engine_to_ecef() * self.global_transform
	for tileset in self.tilesets:
		if (tileset == null): continue
		tileset.update_tileset(camera_xform)
