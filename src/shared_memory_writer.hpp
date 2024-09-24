#pragma once

#include "libsharedmemory.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>

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

    static SharedMemoryWriter* new_from();
    void init();
    void _ready() override;
    void _process(double delta) override;
};
    
}