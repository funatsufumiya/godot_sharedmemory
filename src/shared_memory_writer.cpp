#include "shared_memory_writer.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SharedMemoryWriter::_bind_methods() {
    ClassDB::bind_static_method("SharedMemoryWriter", D_METHOD("try_create", "name", "size", "is_persistent"), &SharedMemoryWriter::try_create);
    ClassDB::bind_method(D_METHOD("write_string", "data"), &SharedMemoryWriter::write_string);
    ClassDB::bind_method(D_METHOD("write_float_array", "data"), &SharedMemoryWriter::write_float_array);
    ClassDB::bind_method(D_METHOD("write_double_array", "data"), &SharedMemoryWriter::write_double_array);
    ClassDB::bind_method(D_METHOD("write_bytes", "data"), &SharedMemoryWriter::write_bytes);
    ClassDB::bind_method(D_METHOD("close"), &SharedMemoryWriter::close);
    ClassDB::bind_method(D_METHOD("destroy"), &SharedMemoryWriter::destroy);
}

SharedMemoryWriter::SharedMemoryWriter()
{
    // UtilityFunctions::print("SharedMemoryWriter::constructor");
}

SharedMemoryWriter::~SharedMemoryWriter()
{
    // UtilityFunctions::print("SharedMemoryWriter::destructor");
}

SharedMemoryWriter* SharedMemoryWriter::try_create(String name, int size, bool is_persistent) {
    // UtilityFunctions::print("SharedMemoryWriter::new_from");

    std::string name_str = name.utf8().get_data();

    std::pair<bool, std::shared_ptr<lsm::SharedMemoryWriteStream>> res
        = lsm::SharedMemoryWriteStream::tryCreate(name_str, size, is_persistent);

    if (res.first == true) {
        auto stream = res.second;
        SharedMemoryWriter* sharedmemory = memnew(SharedMemoryWriter);
        sharedmemory->init(name, size, is_persistent, std::move(stream));
        return sharedmemory;
    } else {
        return nullptr;
    }
}

void SharedMemoryWriter::init(String name, int size, bool is_persistent, std::shared_ptr<lsm::SharedMemoryWriteStream> stream) {
    // UtilityFunctions::print("SharedMemoryWriter::init");

    std::string name_str = name.utf8().get_data();
    this->stream = std::move(stream);
}

void SharedMemoryWriter::_ready() {
    UtilityFunctions::print("SharedMemoryWriter::_ready()");
}

void SharedMemoryWriter::_process(double delta) {
    // UtilityFunctions::print("SharedMemoryWriter::_process()");
}

void SharedMemoryWriter::write_string(String data) {
    std::string str = data.utf8().get_data();
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryWriter::write_string: stream is null");
        return;
    }
    stream->write(str);
}

void SharedMemoryWriter::write_float_array(PackedFloat32Array data) {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryWriter::write_float_array: stream is null");
        return;
    }
    float* arr = data.ptrw();
    size_t size = data.size();
    stream->write(arr, size);
}

void SharedMemoryWriter::write_double_array(PackedFloat64Array data) {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryWriter::write_double_array: stream is null");
        return;
    }
    double* arr = data.ptrw();
    size_t size = data.size();
    stream->write(arr, size);
}

void SharedMemoryWriter::write_bytes(PackedByteArray data) {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryWriter::write_bytes: stream is null");
        return;
    }

    // convert to std::string, and write as string
    auto ptr = data.ptrw();
    size_t size = data.size();

    std::string str(reinterpret_cast<char*>(ptr), size);
    stream->write(str);
}

void SharedMemoryWriter::close() {
    stream->close();
}

void SharedMemoryWriter::destroy() {
    stream->destroy();
}