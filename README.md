# godot_sharedmemory

Shared Memory addon for Godot 4.2.2 - 4.3, using [kyr0/libsharedmemory](https://github.com/kyr0/libsharedmemory).

## Install

use [godot_sharedmemory_bin](https://github.com/funatsufumiya/godot_sharedmemory_bin). see [instruction](https://github.com/funatsufumiya/godot_sharedmemory_bin/blob/main/README.md)

## API

### SharedMemoryReader

- `tryCreate(name: String, size: int, isPersistent: bool) -> SharedMemoryReader`
    - Static method.
    - Create a shared memory reader. If failed, return `null`.
- `close()`
    - Close the shared memory.
- `read_string() -> String`
    - Read a string from shared memory.
- `read_float_array() -> PackedFloat32Array`
- `read_double_array() -> PackedFloat64Array`
- `read_bytes() -> PackedByteArray`
- `read_flags() -> int`
    - Returns the type of the data in the shared memory.
    - compose of the following flags:
        - kMemoryChanged = 1,
        - kMemoryTypeString = 2,
        - kMemoryTypeFloat = 4,
        - kMemoryTypeDouble = 8,
- `read_length() -> int`
    - Returns the length of the data in the shared memory. (returns array size or string length, not byte size)
- `read_size() -> int`
    - Returns the size of the shared memory. (not divided by type size. returns in bytes)

### SharedMemoryWriter

- `tryCreate(name: String, size: int, isPersistent: bool) -> SharedMemoryWriter`
    - Static method.
    - Create a shared memory writer. If failed, return `null`.
- `close()`
  - Close the shared memory (the memory data will only be deleted if `isPersistent` is `false`).
- `destroy()`
    - Destroy the shared memory.
- `write_string(value: String)`
- `write_float_array(value: PackedFloat32Array)`
- `write_double_array(value: PackedFloat64Array)`
- `write_bytes(value: PackedByteArray)`

## Example Project

see [`project/`](project) directory, or [GDScript of demo project](https://github.com/funatsufumiya/godot_sharedmemory/blob/main/project/sharedmemory_test.gd)


## Build and Run

(This process is needed only if you build this plugin by your own)

```bash
$ git submodule update --init --recursive --recommend-shallow --depth 1
$ scons
$ scons target=template_release
$ godot project/project.godot # (only first time)
$ godot --path project/ # run demo
```
