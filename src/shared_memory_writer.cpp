#include "shared_memory_writer.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SharedMemoryWriter::_bind_methods() {
    ClassDB::bind_static_method("SharedMemoryWriter", D_METHOD("create"), &SharedMemoryWriter::create);
    ClassDB::bind_method(D_METHOD("init"), &SharedMemoryWriter::init);
}

SharedMemoryWriter::SharedMemoryWriter()
{
    // UtilityFunctions::print("SharedMemoryWriter::constructor");
}

SharedMemoryWriter::~SharedMemoryWriter()
{
    // UtilityFunctions::print("SharedMemoryWriter::destructor");
}

SharedMemoryWriter* SharedMemoryWriter::create() {
    // UtilityFunctions::print("SharedMemoryWriter::create");

    SharedMemoryWriter* sharedmemory = memnew(SharedMemoryWriter);
    sharedmemory->init();

    return sharedmemory;
}

void SharedMemoryWriter::init() {
    UtilityFunctions::print("SharedMemoryWriter::init");
}

void SharedMemoryWriter::_ready() {
    UtilityFunctions::print("SharedMemoryWriter::_ready()");
}

void SharedMemoryWriter::_process(double delta) {
    // UtilityFunctions::print("SharedMemoryWriter::_process()");
}