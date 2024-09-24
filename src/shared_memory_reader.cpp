#include "shared_memory_reader.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SharedMemoryReader::_bind_methods() {
    ClassDB::bind_static_method("SharedMemoryReader", D_METHOD("new_from"), &SharedMemoryReader::new_from);
    ClassDB::bind_method(D_METHOD("init"), &SharedMemoryReader::init);
}

SharedMemoryReader::SharedMemoryReader()
{
    // UtilityFunctions::print("SharedMemoryReader::constructor");
}

SharedMemoryReader::~SharedMemoryReader()
{
    // UtilityFunctions::print("SharedMemoryReader::destructor");
}

SharedMemoryReader* SharedMemoryReader::new_from() {
    // UtilityFunctions::print("SharedMemoryReader::new_from");

    SharedMemoryReader* sharedmemory = memnew(SharedMemoryReader);
    sharedmemory->init();

    return sharedmemory;
}

void SharedMemoryReader::init() {
    UtilityFunctions::print("SharedMemoryReader::init");
}

void SharedMemoryReader::_ready() {
    UtilityFunctions::print("SharedMemoryReader::_ready()");
}

void SharedMemoryReader::_process(double delta) {
    // UtilityFunctions::print("SharedMemoryReader::_process()");
}