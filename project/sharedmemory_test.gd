class_name SharedMemoryTest
extends Node

enum TestMode {
	STRING,
	BYTES,
	FLOATS,
	DOUBLES
}

@export var mode: TestMode = TestMode.STRING

@onready var writer:SharedMemoryWriter = null
@onready var reader:SharedMemoryReader = null

func testModeToString(mode: TestMode) -> String:
	match mode:
		TestMode.STRING:
			return "STRING"
		TestMode.BYTES:
			return "BYTES"
		TestMode.FLOATS:
			return "FLOATS"
		TestMode.DOUBLES:
			return "DOUBLES"
	return "UNKNOWN"

func _ready():
	# read command line arguments
	var args = OS.get_cmdline_args()
	for arg in args:
		if arg == "--test_string" || arg == "--test-string":
			mode = TestMode.STRING
		elif arg == "--test_bytes" || arg == "--test-bytes":
			mode = TestMode.BYTES
		elif arg == "--test_floats" || arg == "--test-floats":
			mode = TestMode.FLOATS
		elif arg == "--test_doubles" || arg == "--test-doubles":
			mode = TestMode.DOUBLES

	print("Test mode: ", testModeToString(mode))

	match mode:
		TestMode.STRING:
			_test_string()
		TestMode.BYTES:
			_test_bytes()
		TestMode.FLOATS:
			_test_floats()
		TestMode.DOUBLES:
			_test_doubles()	

func _test_string():
	writer = SharedMemoryWriter.try_create("strPipe", 65535, false)
	if writer == null:
		print("Failed to create shared memory writer")
		return

	var data: String = "Hello, World!"
	writer.write_string(data)
	print("wrote: ", data)

	reader = SharedMemoryReader.try_create("strPipe", 65535, false)

	if reader == null:
		print("Failed to create shared memory reader")
		return
	
	var str: String = reader.read_string()
	print("read: ", str)

	var i = 0
	while true:
		await get_tree().create_timer(1.0).timeout

		i += 1
		writer.write_string("Hello, World! " + str(i))
		print("wrote: ", "Hello, World! " + str(i))

		str = reader.read_string()
		print("read: ", str)

func _test_bytes():
	writer = SharedMemoryWriter.try_create("bytePipe", 65535, false)
	if writer == null:
		print("Failed to create shared memory writer")
		return

	var data: PackedByteArray = PackedByteArray()
	for i in range(0, 10):
		data.push_back(i)
	writer.write_bytes(data)
	print("wrote: ", data)

	reader = SharedMemoryReader.try_create("bytePipe", 65535, false)

	if reader == null:
		print("Failed to create shared memory reader")
		return

	var bytes: PackedByteArray = reader.read_bytes()
	print("read: ", bytes)

	var i = 0
	while true:
		await get_tree().create_timer(1.0).timeout

		i += 1
		data = PackedByteArray()
		for j in range(0, 10):
			var random = randi() % 256
			data.push_back(random)
		writer.write_bytes(data)
		print("wrote: ", data)

		bytes = reader.read_bytes()
		print("read: ", bytes)

func _test_floats():
	writer = SharedMemoryWriter.try_create("floatPipe", 65535, false)
	if writer == null:
		print("Failed to create shared memory writer")
		return

	var data: PackedFloat32Array = PackedFloat32Array()
	for i in range(0, 10):
		data.push_back(i)
	writer.write_float_array(data)
	print("wrote: ", data)

	reader = SharedMemoryReader.try_create("floatPipe", 65535, false)

	if reader == null:
		print("Failed to create shared memory reader")
		return

	print("flags: ", reader.read_flags())
	print("length: ", reader.read_length())
	print("size: ", reader.read_size())

	var floats: PackedFloat32Array = reader.read_float_array()
	print("read: ", floats)

	var i = 0
	while true:
		await get_tree().create_timer(1.0).timeout

		i += 1
		data = PackedFloat32Array()
		for j in range(0, 10):
			var random = randf() * 100
			data.push_back(random)
		writer.write_float_array(data)
		print("wrote: ", data)

		floats = reader.read_float_array()
		print("read: ", floats)

func _test_doubles():
	writer = SharedMemoryWriter.try_create("doublePipe", 65535, false)
	if writer == null:
		print("Failed to create shared memory writer")
		return

	var data: PackedFloat64Array = PackedFloat64Array()
	for i in range(0, 10):
		data.push_back(i)
	writer.write_double_array(data)
	print("wrote: ", data)

	reader = SharedMemoryReader.try_create("doublePipe", 65535, false)

	if reader == null:
		print("Failed to create shared memory reader")
		return

	var doubles: PackedFloat64Array = reader.read_double_array()
	print("read: ", doubles)

	var i = 0
	while true:
		await get_tree().create_timer(1.0).timeout

		i += 1
		data = PackedFloat64Array()
		for j in range(0, 10):
			var random = randf() * 100
			data.push_back(random)
		writer.write_double_array(data)
		print("wrote: ", data)

		doubles = reader.read_double_array()
		print("read: ", doubles)

func _exit_tree():
	if writer != null:
		writer.close()
	
	if reader != null:
		reader.close()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
