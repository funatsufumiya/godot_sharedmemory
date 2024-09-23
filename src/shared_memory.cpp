#include "shared_memory.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SharedMemory::_bind_methods() {
    ClassDB::bind_static_method("SharedMemory", D_METHOD("new_from"), &SharedMemory::new_from);
    ClassDB::bind_method(D_METHOD("init"), &SharedMemory::init);
}

SharedMemory::SharedMemory()
{
    // UtilityFunctions::print("SharedMemory::constructor");
}

SharedMemory::~SharedMemory()
{
    // UtilityFunctions::print("SharedMemory::destructor");
}

SharedMemory* SharedMemory::new_from() {
    // UtilityFunctions::print("SharedMemory::new_from");

    SharedMemory* sharedmemory = memnew(SharedMemory);
    sharedmemory->init();

    return sharedmemory;
}

void SharedMemory::init() {
    UtilityFunctions::print("SharedMemory::init");
}

void SharedMemory::_ready() {
    UtilityFunctions::print("SharedMemory::_ready()");
}

void SharedMemory::_process(double delta) {
    // UtilityFunctions::print("SharedMemory::_process()");
}