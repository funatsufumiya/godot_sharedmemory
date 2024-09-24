#pragma once

#include "libsharedmemory.hpp"

#include <godot_cpp/classes/node.hpp>

#include <memory>

namespace godot {

class SharedMemoryReader : public Node
{
    GDCLASS(SharedMemoryReader, Node);

protected:
    static void _bind_methods();

public:
    SharedMemoryReader();
    ~SharedMemoryReader();

    std::shared_ptr<lsm::SharedMemoryReadStream> stream;

    /// Tries to create a shared memory segment for reading.
    /// returns nullptr if the segment could not be opened.
    static SharedMemoryReader* try_create(String name, int size, bool is_persistent);

    void init(String name, int size, bool is_persistent, std::shared_ptr<lsm::SharedMemoryReadStream> stream);
    void _ready() override;
    void _process(double delta) override;

    String read_string();
    PackedFloat32Array read_float_array();
    PackedFloat64Array read_double_array();
    PackedByteArray read_bytes();

    /// Returns the type of the data in the shared memory.
    ///  compose of the following flags:
    /// kMemoryChanged = 1,
    /// kMemoryTypeString = 2,
    /// kMemoryTypeFloat = 4,
    /// kMemoryTypeDouble = 8,
    int read_flags();

    uint64_t read_length();
    uint64_t read_size();
    void close();
};
    
}