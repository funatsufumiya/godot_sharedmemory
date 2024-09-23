# godot_sharedmemory

SharedMemory addon for Godot 4.2.2 - 4.3, using [kyr0/libsharedmemory](https://github.com/kyr0/libsharedmemory).

## Install

use [godot_sharedmemory_bin](https://github.com/funatsufumiya/godot_sharedmemory_bin). see [instruction](https://github.com/funatsufumiya/godot_sharedmemory_bin/blob/main/README.md)

more example, see [GDScript of demo project](https://github.com/funatsufumiya/godot_sharedmemory/blob/main/project/sharedmemory_reader.gd)

## Example Project

see [`project/`](project) directory

## Build and Run

(This process is needed only if you build this plugin by your own)

```bash
$ git submodule update --init --recursive --recommend-shallow --depth 1
$ scons
$ scons target=template_release
$ godot project/project.godot # (only first time)
$ godot --path project/ # run demo
```
