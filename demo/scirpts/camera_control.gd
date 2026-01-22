extends Camera3D

@export var move_speed := 10.0
@export var fast_multiplier := 4.0
@export var mouse_sensitivity := 0.002

var yaw := 0.0
var pitch := 0.0
var looking := false


func _ready():
	yaw = rotation.y
	pitch = rotation.x


func _unhandled_input(event):
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_RIGHT:
			looking = event.pressed
			Input.set_mouse_mode(
				Input.MOUSE_MODE_CAPTURED if looking
				else Input.MOUSE_MODE_VISIBLE
			)

		elif event.button_index == MOUSE_BUTTON_WHEEL_UP and event.pressed:
			move_speed *= 1.1
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN and event.pressed:
			move_speed /= 1.1

	elif event is InputEventMouseMotion and looking:
		yaw   -= event.relative.x * mouse_sensitivity
		pitch -= event.relative.y * mouse_sensitivity

		pitch = clamp(pitch, -1.55, 1.55)
		rotation = Vector3(pitch, yaw, 0.0)


func _process(delta):
	if not looking:
		return

	var direction := Vector3.ZERO

	if Input.is_key_pressed(KEY_W):
		direction -= transform.basis.z
	if Input.is_key_pressed(KEY_S):
		direction += transform.basis.z
	if Input.is_key_pressed(KEY_A):
		direction -= transform.basis.x
	if Input.is_key_pressed(KEY_D):
		direction += transform.basis.x
	if Input.is_key_pressed(KEY_E):
		direction += transform.basis.y
	if Input.is_key_pressed(KEY_Q):
		direction -= transform.basis.y

	if direction != Vector3.ZERO:
		direction = direction.normalized()

	var speed := move_speed
	if Input.is_key_pressed(KEY_SHIFT):
		speed *= fast_multiplier

	global_position += direction * speed * delta
