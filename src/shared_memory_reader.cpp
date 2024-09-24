#include "shared_memory_reader.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SharedMemoryReader::_bind_methods() {
    ClassDB::bind_static_method("SharedMemoryReader", D_METHOD("try_create", "name", "size", "is_persistent"), &SharedMemoryReader::try_create);
    ClassDB::bind_method(D_METHOD("read_string"), &SharedMemoryReader::read_string);
    ClassDB::bind_method(D_METHOD("read_float_array"), &SharedMemoryReader::read_float_array);
    ClassDB::bind_method(D_METHOD("read_double_array"), &SharedMemoryReader::read_double_array);
    ClassDB::bind_method(D_METHOD("read_bytes"), &SharedMemoryReader::read_bytes);
    ClassDB::bind_method(D_METHOD("read_flags"), &SharedMemoryReader::read_flags);
    ClassDB::bind_method(D_METHOD("read_length"), &SharedMemoryReader::read_length);
    ClassDB::bind_method(D_METHOD("read_size"), &SharedMemoryReader::read_size);
    ClassDB::bind_method(D_METHOD("close"), &SharedMemoryReader::close);
}

SharedMemoryReader::SharedMemoryReader()
{
    // UtilityFunctions::print("SharedMemoryReader::constructor");
}

SharedMemoryReader::~SharedMemoryReader()
{
    // UtilityFunctions::print("SharedMemoryReader::destructor");
}

SharedMemoryReader* SharedMemoryReader::try_create(String name, int size, bool is_persistent) {
    // UtilityFunctions::print("SharedMemoryReader::new_from");

    std::string name_str = name.utf8().get_data();

    std::pair<bool, std::shared_ptr<lsm::SharedMemoryReadStream>> res
        = lsm::SharedMemoryReadStream::tryCreate(name_str, size, is_persistent);

    if (res.first == true) {
        auto stream = res.second;
        SharedMemoryReader* sharedmemory = memnew(SharedMemoryReader);
        sharedmemory->init(name, size, is_persistent, std::move(stream));
        return sharedmemory;
    } else {
        return nullptr;
    }
}

void SharedMemoryReader::init(String name, int size, bool is_persistent, std::shared_ptr<lsm::SharedMemoryReadStream> stream) {
    // UtilityFunctions::print("SharedMemoryReader::init");

    std::string name_str = name.utf8().get_data();
    this->stream = std::move(stream);
}

void SharedMemoryReader::_ready() {
    UtilityFunctions::print("SharedMemoryReader::_ready()");
}

void SharedMemoryReader::_process(double delta) {
    // UtilityFunctions::print("SharedMemoryReader::_process()");
}

String SharedMemoryReader::read_string() {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryReader::read_string: stream is null");
        return "";
    }
    std::string str = stream->readString();
    return str.c_str();
}

PackedFloat32Array SharedMemoryReader::read_float_array() {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryReader::read_float_array: stream is null");
        return PackedFloat32Array();
    }

    float* data = stream->readFloatArray();
    size_t length = stream->readLength(lsm::kMemoryTypeFloat);

    PackedFloat32Array arr;
    arr.resize(length);

    auto ptrw = arr.ptrw();
    memcpy(ptrw, data, length * sizeof(float));

    delete[] data;

    return arr;
}

PackedFloat64Array SharedMemoryReader::read_double_array() {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryReader::read_double_array: stream is null");
        return PackedFloat64Array();
    }

    double* data = stream->readDoubleArray();
    size_t length = stream->readLength(lsm::kMemoryTypeDouble);

    PackedFloat64Array arr;
    arr.resize(length);

    auto ptrw = arr.ptrw();
    memcpy(ptrw, data, length * sizeof(double));

    delete[] data;

    return arr;
}

PackedByteArray SharedMemoryReader::read_bytes() {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryReader::read_bytes: stream is null");
        return PackedByteArray();
    }

    std::string str = stream->readString();
    
    PackedByteArray arr;
    arr.resize(str.size());

    auto ptrw = arr.ptrw();
    memcpy(ptrw, str.c_str(), str.size());

    return arr;
}

int SharedMemoryReader::read_flags() {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryReader::read_type: stream is null");
        return 0;
    }
    return stream->readFlags();
}

uint64_t SharedMemoryReader::read_length() {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryReader::read_length: stream is null");
        return 0;
    }
    return stream->readLength(stream->readFlags());
}

uint64_t SharedMemoryReader::read_size() {
    if (stream == nullptr) {
        UtilityFunctions::push_error("SharedMemoryReader::read_size: stream is null");
        return 0;
    }
    return stream->readSize(stream->readFlags());
}

void SharedMemoryReader::close() {
    stream->close();
}