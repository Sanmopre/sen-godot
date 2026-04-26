@icon("res://addons/cesium_godot/resources/icons/video.svg")

class_name DynamicCamera extends BaseCamera

@export
var move_speed: float = 100.0

var offset_speed: float = 0.0

@export
var rotation_speed: float = 0.005

@export var follow_offset: Vector3 = Vector3.ZERO:
	set(value):
		follow_offset = value
		_sync_boxes_from_follow_offset()

@export var follow_rotation_offset_degrees: Vector3 = Vector3.ZERO:
	set(value):
		follow_rotation_offset_degrees = value
		_sync_rotation_boxes_from_follow_rotation_offset()

# X = pitch, Y = yaw, Z = roll

var desired_cam_pos: Vector3 = Vector3.ZERO
var is_moving_physical: bool = false
var surface_basis: Basis
var curr_yaw: float
var curr_pitch: float
var moving_direction: Vector3
var speed_multiplier: float = 1.0

@export var entity_to_follow: Node3D = null
@export var entity_to_follow_ecef: Vector3 = Vector3.ZERO

# UI Elements
@export var free_view_button: Button
@export var entity_list : ItemList

@export var x_box: SpinBox
@export var y_box: SpinBox
@export var z_box: SpinBox

@export var rot_x_box: SpinBox
@export var rot_y_box: SpinBox
@export var rot_z_box: SpinBox

func _ready() -> void:
	super._ready()

	if x_box != null:
		x_box.value_changed.connect(_on_follow_offset_changed)
	if y_box != null:
		y_box.value_changed.connect(_on_follow_offset_changed)
	if z_box != null:
		z_box.value_changed.connect(_on_follow_offset_changed)

	if rot_x_box != null:
		rot_x_box.value_changed.connect(_on_follow_rotation_offset_changed)
	if rot_y_box != null:
		rot_y_box.value_changed.connect(_on_follow_rotation_offset_changed)
	if rot_z_box != null:
		rot_z_box.value_changed.connect(_on_follow_rotation_offset_changed)

	if free_view_button != null:
		free_view_button.pressed.connect(_on_free_view_button_pressed)

	_sync_boxes_from_follow_offset()
	_sync_rotation_boxes_from_follow_rotation_offset()

func _on_free_view_button_pressed() -> void:
	entity_to_follow = null
	if entity_list != null:
		entity_list.deselect_all()

func _on_follow_offset_changed(_value: float) -> void:
	if x_box == null or y_box == null or z_box == null:
		return

	follow_offset = Vector3(
		x_box.value,
		y_box.value,
		z_box.value
	)

func _sync_boxes_from_follow_offset() -> void:
	if x_box == null or y_box == null or z_box == null:
		return

	x_box.set_block_signals(true)
	y_box.set_block_signals(true)
	z_box.set_block_signals(true)

	x_box.value = follow_offset.x
	y_box.value = follow_offset.y
	z_box.value = follow_offset.z

	x_box.set_block_signals(false)
	y_box.set_block_signals(false)
	z_box.set_block_signals(false)

func _on_follow_rotation_offset_changed(_value: float) -> void:
	if rot_x_box == null or rot_y_box == null or rot_z_box == null:
		return

	follow_rotation_offset_degrees = Vector3(
		rot_x_box.value,
		rot_y_box.value,
		rot_z_box.value
	)

func _sync_rotation_boxes_from_follow_rotation_offset() -> void:
	if rot_x_box == null or rot_y_box == null or rot_z_box == null:
		return

	rot_x_box.set_block_signals(true)
	rot_y_box.set_block_signals(true)
	rot_z_box.set_block_signals(true)

	rot_x_box.value = follow_rotation_offset_degrees.x
	rot_y_box.value = follow_rotation_offset_degrees.y
	rot_z_box.value = follow_rotation_offset_degrees.z

	rot_x_box.set_block_signals(false)
	rot_y_box.set_block_signals(false)
	rot_z_box.set_block_signals(false)

func _physics_process(_delta: float) -> void:
	if entity_to_follow != null:
        
		# Rebase the georeference origin to the entity's true ECEF position
		self.globe_node.ecefX = entity_to_follow_ecef.x
		self.globe_node.ecefY = entity_to_follow_ecef.y
		self.globe_node.ecefZ = entity_to_follow_ecef.z

		var target_transform := entity_to_follow.global_transform
		var target_basis := target_transform.basis.orthonormalized()

		if target_basis.determinant() < 0.0:
			target_basis.x = -target_basis.x

		var target_pos := target_transform.origin + target_basis * follow_offset

		var rot_offset_basis := Basis()
		rot_offset_basis = rot_offset_basis.rotated(Vector3.RIGHT, deg_to_rad(follow_rotation_offset_degrees.x))
		rot_offset_basis = rot_offset_basis.rotated(Vector3.UP, deg_to_rad(follow_rotation_offset_degrees.y))
		rot_offset_basis = rot_offset_basis.rotated(Vector3.BACK, deg_to_rad(follow_rotation_offset_degrees.z))

		var final_basis := (target_basis * rot_offset_basis).orthonormalized()
		global_transform = Transform3D(final_basis, target_pos)
		return

	self.surface_basis = self.calculate_surface_basis()
	self.update_camera_rotation()
	
	self.move_speed = self.move_speed + self.offset_speed
	
	if self.globe_node.origin_type == CesiumGeoreference.OriginType.TrueOrigin:
		self.camera_walk_physical(self.moving_direction)
		self.update_camera_pos_physical()
	else:
		var ecefDir: Vector3 = self.globe_node.get_initial_tx_engine_to_ecef() * self.moving_direction
		camera_walk_ecef(-ecefDir.normalized())

func _process(delta: float) -> void:
	super(delta)
	movement_input(delta)

func calculate_surface_basis() -> Basis:
	var cam_ecef_pos: Vector3
	if self.globe_node.origin_type == CesiumGeoreference.CartographicOrigin:
		cam_ecef_pos = Vector3(self.globe_node.ecefX, self.globe_node.ecefY, self.globe_node.ecefZ)
	else:
		cam_ecef_pos = self.globe_node.get_tx_engine_to_ecef() * self.global_position
	
	var up: Vector3 = self.globe_node.get_normal_at_surface_pos(cam_ecef_pos)
	
	var reference = -self.global_basis.z
	
	var dotProduct := up.dot(reference)
	
	if dotProduct > 0.99:
		reference = self.global_basis.x
	
	var right := up.cross(reference).normalized()
	var forward := right.cross(up).normalized()
	var result := Basis(right, up, -forward)
	return result

func movement_input(delta: float) -> void:
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		var mouse_velocity: Vector2 = Input.get_last_mouse_velocity()
		var delta_yaw: float = mouse_velocity.x * delta * self.rotation_speed
		var delta_pitch: float = mouse_velocity.y * delta * self.rotation_speed
		self.rotate_camera(delta_pitch, delta_yaw)
	
	# Calculate speed multiplier based on Shift and Control
	speed_multiplier = 1.0
	if Input.is_key_pressed(KEY_SHIFT):
		speed_multiplier *= 4.0
	if Input.is_key_pressed(KEY_CTRL):
		speed_multiplier *= 0.15
	
	if entity_to_follow == null:
		var direction := Vector3.ZERO
		var movingBasis: Basis = self.global_transform.basis

		if Input.is_key_pressed(KEY_KP_ADD) or Input.is_key_pressed(KEY_PLUS):
			self.offset_speed += self.move_speed * 0.1 * delta
		if Input.is_key_pressed(KEY_KP_SUBTRACT) or Input.is_key_pressed(KEY_MINUS):
			self.offset_speed = maxf(self.offset_speed - self.move_speed * 0.1 * delta, 0.0)

		if Input.is_key_pressed(KEY_Q):
			direction -= movingBasis.y
		if Input.is_key_pressed(KEY_E):
			direction += movingBasis.y

		if Input.is_key_pressed(KEY_W):
			direction -= movingBasis.z
		if Input.is_key_pressed(KEY_S):
			direction += movingBasis.z

		if Input.is_key_pressed(KEY_D):
			direction += movingBasis.x
		if Input.is_key_pressed(KEY_A):
			direction -= movingBasis.x
		if Input.is_key_pressed(KEY_KP_6):
			rotate_z(delta * 0.5)
		if Input.is_key_pressed(KEY_KP_4):
			rotate_z(-delta * 0.5)
			
		self.moving_direction = direction.normalized()
	else:
		# Adjust follow offsets for free movement around entity
		var target_transform := entity_to_follow.global_transform
		var target_basis := target_transform.basis.orthonormalized()
		if target_basis.determinant() < 0.0:
			target_basis.x = -target_basis.x
		
		var delta_world := Vector3.ZERO
		if Input.is_key_pressed(KEY_W):
			delta_world -= self.global_basis.z * move_speed * delta * speed_multiplier
		if Input.is_key_pressed(KEY_S):
			delta_world += self.global_basis.z * move_speed * delta * speed_multiplier
		if Input.is_key_pressed(KEY_A):
			delta_world -= self.global_basis.x * move_speed * delta * speed_multiplier
		if Input.is_key_pressed(KEY_D):
			delta_world += self.global_basis.x * move_speed * delta * speed_multiplier
		if Input.is_key_pressed(KEY_Q):
			delta_world -= self.global_basis.y * move_speed * delta * speed_multiplier
		if Input.is_key_pressed(KEY_E):
			delta_world += self.global_basis.y * move_speed * delta * speed_multiplier
		
		var delta_local := target_basis.inverse() * delta_world
		follow_offset += delta_local
		
		# Adjust rotation offsets
		var delta_rot := Vector3.ZERO
		if Input.is_key_pressed(KEY_KP_6):
			delta_rot.z += delta * 30.0  # degrees per second
		if Input.is_key_pressed(KEY_KP_4):
			delta_rot.z -= delta * 30.0
		
		follow_rotation_offset_degrees += delta_rot

func camera_walk_ecef(direction: Vector3) -> void:
	if direction == Vector3.ZERO:
		return
	direction *= -self.move_speed * self.speed_multiplier
	
	self.globe_node.ecefX += direction.x
	self.globe_node.ecefY += direction.y
	self.globe_node.ecefZ += direction.z

func camera_walk_physical(direction: Vector3) -> void:
	if desired_cam_pos == Vector3.ZERO:
		self.desired_cam_pos = self.global_position + direction * self.move_speed * self.speed_multiplier

	self.desired_cam_pos += direction * self.move_speed * self.speed_multiplier
	self.is_moving_physical = direction != Vector3.ZERO

func update_camera_pos_physical() -> void:
	if self.is_moving_physical:
		self.global_position = self.desired_cam_pos

func update_camera_rotation() -> void:
	var moddedBasis: Basis = self.surface_basis.rotated(self.surface_basis.y.normalized(), -curr_yaw)
	moddedBasis = moddedBasis.rotated(moddedBasis.x, curr_pitch)
	moddedBasis.x = -moddedBasis.x

	self.basis = moddedBasis
	self.curr_yaw = 0

func rotate_camera(delta_pitch: float, delta_yaw: float) -> void:
	if entity_to_follow != null:
		# Adjust rotation offsets for attached mode
		follow_rotation_offset_degrees.y -= rad_to_deg(delta_yaw)
		follow_rotation_offset_degrees.x -= rad_to_deg(delta_pitch)
		return
	
	self.curr_yaw += delta_yaw

	var camera_forward: Vector3 = -self.global_basis.z.normalized()
	var surface_forward: Vector3 = self.surface_basis.z.normalized()

	var cross = camera_forward.cross(surface_forward)
	var dot = camera_forward.dot(surface_forward)
	var unsigned_angle = rad_to_deg(acos(clamp(dot, -1.0, 1.0)))

	var camera_right = self.global_basis.x.normalized()
	var signed_angle = unsigned_angle * sign(cross.dot(camera_right))

	var desired_pitch = self.curr_pitch + delta_pitch

	if signed_angle > -110 and signed_angle < 0 and delta_pitch > 0:
		return
	if signed_angle < 110 and signed_angle > 0 and delta_pitch < 0:
		return
	self.curr_pitch = desired_pitch

func _get_surface_distance_raycast() -> float:
	var space_state = get_world_3d().direct_space_state

	var ray_query = PhysicsRayQueryParameters3D.new()
	ray_query.from = global_position
	if self.globe_node.origin_type == CesiumGeoreference.CartographicOrigin:
		ray_query.to = global_position + (-surface_basis.y * RADII * 2)
	else:
		ray_query.to = self.globe_node.global_position
	ray_query.hit_from_inside = true
	ray_query.hit_back_faces = true
	ray_query.exclude = [self]
	ray_query.collision_mask = 1

	var result: Dictionary = space_state.intersect_ray(ray_query)

	ray_query.to = global_position + (moving_direction * self.move_speed * 10)
	ray_query.hit_from_inside = false
	var secondResult: Dictionary = space_state.intersect_ray(ray_query)

	var distanceToFloor: float = RADII
	if result:
		distanceToFloor = global_position.distance_to(result.position)
		last_hit_distance = distanceToFloor

	var distanceToMove: float = RADII
	if secondResult:
		distanceToMove = global_position.distance_to(secondResult.position)
		if distanceToMove < 10:
			self.moving_direction = Vector3.ZERO
		last_hit_distance = distanceToMove

	var closest_distance: float = distanceToFloor
	if distanceToMove < closest_distance:
		closest_distance = distanceToMove

	return closest_distance