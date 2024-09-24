#pragma once

#include "libsharedmemory.hpp"

#include <godot_cpp/classes/node.hpp>

#include <memory>

namespace godot {

class SharedMemoryWriter : public Node
{
    GDCLASS(SharedMemoryWriter, Node);

protected:
    static void _bind_methods();

public:
    SharedMemoryWriter();
    ~SharedMemoryWriter();

    std::shared_ptr<lsm::SharedMemoryWriteStream> stream;

    /// Tries to create a shared memory segment for writing.
    /// returns nullptr if the segment could not be opened.
    static SharedMemoryWriter* try_create(String name, int size, bool is_persistent);

    void init(String name, int size, bool is_persistent, std::shared_ptr<lsm::SharedMemoryWriteStream> stream);
    void _ready() override;
    void _process(double delta) override;

    void write_string(String data);
    void write_float_array(PackedFloat32Array data);
    void write_double_array(PackedFloat64Array data);
    void write_bytes(PackedByteArray data);
    void close();
    void destroy();
};
    
}