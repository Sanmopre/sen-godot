class_name SenCamera
extends Camera3D

var globe_node: CesiumGeoreference
var target: Node3D = null

@export var target_path: NodePath:
	set(value):
		target_path = value
		_update_target()

@export var offset := Vector3(0, 0, 0)
@export var pitch_offset := 0.0  # X
@export var yaw_offset := 0.0    # Y
@export var roll_offset := 0.0   # Z

@export var rotation_offset_degrees := Vector3(0, 0, 0)
@export var tilesets: Array[Node]

func _ready() -> void:
	globe_node = get_tree().current_scene.get_node("georeference") as CesiumGeoreference
	if globe_node == null:
		push_error("CesiumGeoreference not found.")
		return

	far = 149597870700.0 + 3 * 1392700000.0

func _process(delta):
	if target:
		_update_tilesets()

		var t = target.global_transform
		var basis = t.basis.orthonormalized()

		if basis.determinant() < 0.0:
			basis.x = -basis.x

		var rot_offset_basis := Basis()
		rot_offset_basis = rot_offset_basis.rotated(Vector3.RIGHT, deg_to_rad(pitch_offset))
		rot_offset_basis = rot_offset_basis.rotated(Vector3.UP, deg_to_rad(yaw_offset))
		rot_offset_basis = rot_offset_basis.rotated(Vector3.BACK, deg_to_rad(roll_offset))

		var final_basis = basis * rot_offset_basis

		var world_offset = final_basis * offset
		var final_pos = t.origin + world_offset

		global_transform = Transform3D(final_basis, final_pos)

func _update_target():
	target = null

	if target_path != NodePath(""):
		target = get_tree().current_scene.get_node_or_null(target_path)

func _update_tilesets() -> void:
	var camera_xform := self.globe_node.get_tx_engine_to_ecef() * self.global_transform

	for node in tilesets:
		if node == null:
			continue

		if node is Cesium3DTileset:
			node.update_tileset(camera_xform)
