extends Camera3D

var globe_node: CesiumGeoreference
var target: Node3D = null
const TARGET_PATH = "georeference/sen_godot_ig_result_A400M_GAF_1/yaw_pivot/pitch_pivot/roll_pivot/model"

func _ready() -> void:
	globe_node = get_tree().current_scene.get_node("georeference") as CesiumGeoreference
	if globe_node == null:
		push_error("CesiumGeoreference not found.")
		return

	far = 149597870700.0 + 3 * 1392700000.0

func _process(delta):
	# Re-acquire if missing or freed
	if target == null or !is_instance_valid(target):
		target = get_tree().current_scene.get_node_or_null(TARGET_PATH)

	if target:
		var t = target.global_transform
		
		# Step 1: remove scale
		var basis = t.basis.orthonormalized()

		# Step 2: FIX mirrored basis (this is the key 🔥)
		if basis.determinant() < 0.0:
			basis.x = -basis.x  # flip one axis to fix handedness

		# Step 3: rebuild transform
		global_transform = Transform3D(basis, t.origin)
