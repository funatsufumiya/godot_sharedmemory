#pragma once

#include "libsharedmemory.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>

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

    static SharedMemoryReader* create();
    void init();
    void _ready() override;
    void _process(double delta) override;
};
    
}