class_name SenCamera
extends Camera3D

var globe_node: CesiumGeoreference
var target: Node3D = null

@export var target_path: NodePath:
	set(value):
		target_path = value
		_update_target()

@export var offset := Vector3.ZERO
@export var pitch_offset := 0.0
@export var yaw_offset := 0.0
@export var roll_offset := 0.0

@export var rotation_offset_degrees := Vector3.ZERO
@export var tilesets: Array[Cesium3DTileset] = []
@export var tileset_name := ""

func _ready() -> void:
	globe_node = get_tree().current_scene.get_node_or_null(^"georeference") as CesiumGeoreference
	if globe_node == null:
		push_error("CesiumGeoreference not found.")
		return

	far = 149597870700.0 + 3.0 * 1392700000.0
	_update_target()
	_ensure_tileset()

func _process(_delta: float) -> void:
	if target == null or globe_node == null:
		return

	_ensure_tileset()
	_update_camera_transform()
	_update_tilesets()

func _update_target() -> void:
	target = null

	if target_path != NodePath():
		target = get_tree().current_scene.get_node_or_null(target_path) as Node3D

func _ensure_tileset() -> void:
	if globe_node == null or tileset_name.is_empty():
		return

	var tileset := globe_node.get_node_or_null(NodePath(tileset_name)) as Cesium3DTileset
	if tileset == null:
		return

	if not tilesets.has(tileset):
		tilesets.append(tileset)

func _update_camera_transform() -> void:
	var t := target.global_transform
	var basis := t.basis.orthonormalized()

	if basis.determinant() < 0.0:
		basis.x = -basis.x

	var rot_offset_basis := Basis()
	rot_offset_basis = rot_offset_basis.rotated(Vector3.RIGHT, deg_to_rad(pitch_offset))
	rot_offset_basis = rot_offset_basis.rotated(Vector3.UP, deg_to_rad(yaw_offset))
	rot_offset_basis = rot_offset_basis.rotated(Vector3.BACK, deg_to_rad(roll_offset))

	var final_basis := basis * rot_offset_basis
	var final_pos := t.origin + final_basis * offset

	global_transform = Transform3D(final_basis, final_pos)

func _update_tilesets() -> void:
	var camera_xform := globe_node.get_tx_engine_to_ecef() * global_transform

	for tileset in tilesets:
		if is_instance_valid(tileset):
			tileset.update_tileset(camera_xform)