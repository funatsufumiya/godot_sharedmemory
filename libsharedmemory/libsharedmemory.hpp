
#ifndef INCLUDE_LIBSHAREDMEMORY_HPP_
#define INCLUDE_LIBSHAREDMEMORY_HPP_

#include <ostream>
#define LIBSHAREDMEMORY_VERSION_MAJOR 0
#define LIBSHAREDMEMORY_VERSION_MINOR 0
#define LIBSHAREDMEMORY_VERSION_PATCH 9

#include <cstdint>
#include <cstring>
#include <string>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif


#include <iostream>
#include <cstddef> // nullptr_t, ptrdiff_t, std::size_t
#include <memory>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <godot_cpp/variant/utility_functions.hpp>

namespace lsm {

enum Error {
  kOK = 0,
  kErrorCreationFailed = 100,
  kErrorMappingFailed = 110,
  kErrorOpeningFailed = 120,
};

enum DataType {
  kMemoryChanged = 1,
  kMemoryTypeString = 2,
  kMemoryTypeFloat = 4,
  kMemoryTypeDouble = 8,
};

inline void throw_error(std::string message) {
    godot::String msg = message.c_str();
    godot::UtilityFunctions::push_error(msg);
    // assert(false);
    abort();
}

inline void push_error(std::string message) {
    godot::String msg = message.c_str();
    godot::UtilityFunctions::push_error(msg);
}

inline std::string errorToString(Error error) {
    switch (error) {
        case kOK:
            return "OK";
        case kErrorCreationFailed:
            return "Error: Creation failed";
        case kErrorMappingFailed:
            return "Error: Mapping failed";
        case kErrorOpeningFailed:
            return "Error: Opening failed";
    }
}

inline void push_memory_error(Error error) {
    std::string message = errorToString(error);
    std::string header = "[SharedMemory] ";
    push_error(header + message);
}

// define my throw function

// byte sizes of memory layout
const size_t bufferSizeSize = 4; // size_t takes 4 bytes
const size_t sizeOfOneFloat = 4; // float takes 4 bytes
const size_t sizeOfOneChar = 1; // char takes 1 byte
const size_t sizeOfOneDouble = 8; // double takes 8 bytes
const size_t flagSize = 1; // char takes 1 byte

class Memory {
public:
    // path should only contain alpha-numeric characters, and is normalized
    // on linux/macOS.
    explicit Memory(std::string path, std::size_t size, bool persist);

    // create a shared memory area and open it for writing
    inline Error create() { return createOrOpen(true); };

    // open an existing shared memory for reading
    inline Error open() { return createOrOpen(false); };

    inline std::size_t size() { return _size; };

    inline const std::string &path() { return _path; }

    inline void *data() { return _data; }

    void destroy();

    void close();

    ~Memory();

private:
    Error createOrOpen(bool create);

    std::string _path;
    void *_data = nullptr;
    std::size_t _size = 0;
    bool _persist = true;
#if defined(_WIN32)
    HANDLE _handle;
#else
    int _fd = -1;
#endif
};

class SharedMemoryReadStream {
public:
    /// Tries to create a shared memory segment for reading.
    /// returns nullptr if the segment could not be opened.
    static std::pair<bool, std::shared_ptr<SharedMemoryReadStream>> tryCreate(const std::string name, const std::size_t bufferSize, const bool isPersistent) {
        auto stream = std::make_shared<SharedMemoryReadStream>(name, bufferSize, isPersistent);
        auto err = stream->getMemoryErrorStatus();
        if (err != kOK) {
            // push_memory_error(err);
            push_error("Shared memory segment could not be opened. (reason: " + errorToString(err) + ")");
            if (err == kErrorMappingFailed) {
                stream->close();
            }
            return std::make_pair(false, nullptr);
        }
        return std::make_pair(true, stream);
    }

    SharedMemoryReadStream(const std::string name, const std::size_t bufferSize, const bool isPersistent): 
        _memory(name, bufferSize, isPersistent) {

        memoryErrorStetus = _memory.open();
        if (memoryErrorStetus != kOK) {
            // throw "Shared memory segment could not be opened.";
            // throw_error("Shared memory segment could not be opened.");
            push_error("Shared memory segment could not be opened. (reason: " + errorToString(memoryErrorStetus) + ")");
        }
    }

    Error getMemoryErrorStatus() {
        return memoryErrorStetus;
    }

    inline char readFlags() {
      char* memory = (char*) _memory.data();
      return memory[0];
    }

    inline void close() { _memory.close(); }


    inline size_t readSize(char dataType) {
        void *memory = _memory.data();
        std::size_t size = 0;

        // TODO(kyr0): should be clarified why we need to use size_t there
        // for the size to be received correctly, but in float, we need int
        // Might be prone to undefined behaviour; should be tested
        // with various compilers; otherwise use memcpy() for the size
        // and align the memory with one cast.

        if (dataType & kMemoryTypeDouble) {
            size_t *intMemory = (size_t *)memory;
            // copy size data to size variable
            std::memcpy(&size, &intMemory[flagSize], bufferSizeSize);
        }

        if (dataType & kMemoryTypeFloat) {
            int* intMemory = (int*) memory; 
            // copy size data to size variable
            std::memcpy(&size, &intMemory[flagSize], bufferSizeSize);
        }

        if (dataType & kMemoryTypeString) {
            char* charMemory = (char*) memory; 
            // copy size data to size variable
            std::memcpy(&size, &charMemory[flagSize], bufferSizeSize);
        }
        return size;
    }

    inline size_t readLength(char dataType) {
      size_t size = readSize(dataType);

      if (dataType & kMemoryTypeString) {
        return size / sizeOfOneChar;
      }
            
      if (dataType & kMemoryTypeFloat) {
        return size / sizeOfOneFloat;
      }
            
      if (dataType & kMemoryTypeDouble) {
        return size / sizeOfOneDouble;
      }
      return 0;
    }

    /**
     * @brief Returns a doible* read from shared memory
     * Caller has the obligation to call delete [] on the returning float*.
     *
     * @return float*
     */
    // TODO: might wanna use templated functions here like: <T> readNumericArray()
    inline double* readDoubleArray() {
        void *memory = _memory.data();
        std::size_t size = readSize(kMemoryTypeDouble);
        double* typedMemory = (double*) memory; 

        // allocating memory on heap (this might leak)
        double *data = new double[size / sizeOfOneDouble]();

        // copy to data buffer
        std::memcpy(data, &typedMemory[flagSize + bufferSizeSize], size);
        
        return data;
    }

    /**
     * @brief Returns a float* read from shared memory
     * Caller has the obligation to call delete [] on the returning float*.
     * 
     * @return float* 
     */
    inline float* readFloatArray() {
        void *memory = _memory.data();
        float *typedMemory = (float *)memory;
        
        std::size_t size = readSize(kMemoryTypeFloat);

        // allocating memory on heap (this might leak)
        float *data = new float[size / sizeOfOneFloat]();

        // copy to data buffer
        std::memcpy(data, &typedMemory[flagSize + bufferSizeSize], size);
        
        return data;
    }

    inline std::string readString() {
        char* memory = (char*) _memory.data();

        std::size_t size = readSize(kMemoryTypeString);

        // create a string that copies the data from memory
        std::string data =
            std::string(&memory[flagSize + bufferSizeSize], size);
        
        return data;
    }

private:
    Memory _memory;
    Error memoryErrorStetus = kOK;
};

class SharedMemoryWriteStream {
public:
    /// Tries to create a shared memory segment for writing.
    /// returns nullptr if the segment could not be created.
    static std::pair<bool, std::shared_ptr<SharedMemoryWriteStream>> tryCreate(const std::string name, const std::size_t bufferSize, const bool isPersistent) {
        auto stream = std::make_shared<SharedMemoryWriteStream>(name, bufferSize, isPersistent);
        auto err = stream->getMemoryErrorStatus();
        if (err != kOK) {
            // push_memory_error(err);
            push_error("Shared memory segment could not be created. (reason: " + errorToString(err) + ")");
            if (err == kErrorMappingFailed) {
                stream->close();
            }
            return std::make_pair(false, nullptr);
        }
        return std::make_pair(true, stream);
    }

    SharedMemoryWriteStream(const std::string name, const std::size_t bufferSize, const bool isPersistent): 
        _memory(name, bufferSize, isPersistent) {

        memoryErrorStetus = _memory.create();
        if (memoryErrorStetus != kOK) {
            // throw "Shared memory segment could not be created.";
            // throw_error("Shared memory segment could not be created.");
            push_error("Shared memory segment could not be created. (reason: " + errorToString(_memory.create()) + ")");
        }
    }

    Error getMemoryErrorStatus() {
        return memoryErrorStetus;
    }

    inline void close() {
      _memory.close();
    }

    // https://stackoverflow.com/questions/18591924/how-to-use-bitmask
    inline char getWriteFlags(const char type,
                              const char currentFlags) {
        char flags = type;

        if ((currentFlags & (kMemoryChanged)) == kMemoryChanged) {
            // disable flag, leave rest untouched
            flags &= ~kMemoryChanged;
        } else {
            // enable flag, leave rest untouched
            flags ^= kMemoryChanged;
        }
        return flags;
    }

    inline void write(const std::string& string) {
        char* memory = (char*) _memory.data();

        // 1) copy change flag into buffer for change detection
        char flags = getWriteFlags(kMemoryTypeString, ((char*) _memory.data())[0]);
        std::memcpy(&memory[0], &flags, flagSize);

        // 2) copy buffer size into buffer (meta data for deserializing)
        const char *stringData = string.data();
        const std::size_t bufferSize = string.size();

        // write data
        std::memcpy(&memory[flagSize], &bufferSize, bufferSizeSize);

        // 3) copy stringData into memory buffer
        std::memcpy(&memory[flagSize + bufferSizeSize], stringData, bufferSize);
    }

    // TODO: might wanna use template function here for numeric arrays,
    // like void writeNumericArray(<T*> data, std::size_t length)
    inline void write(float* data, std::size_t length) {
        float* memory = (float*) _memory.data();

        char flags = getWriteFlags(kMemoryTypeFloat, ((char*) _memory.data())[0]);
        std::memcpy(&memory[0], &flags, flagSize);

        // 2) copy buffer size into buffer (meta data for deserializing)
        const std::size_t bufferSize = length * sizeOfOneFloat;
        std::memcpy(&memory[flagSize], &bufferSize, bufferSizeSize);
        
        // 3) copy float* into memory buffer
        std::memcpy(&memory[flagSize + bufferSizeSize], data, bufferSize);
    }

    inline void write(double* data, std::size_t length) {
        double* memory = (double*) _memory.data();

        char flags = getWriteFlags(kMemoryTypeDouble, ((char*) _memory.data())[0]);
        std::memcpy(&memory[0], &flags, flagSize);

        // 2) copy buffer size into buffer (meta data for deserializing)
        const std::size_t bufferSize = length * sizeOfOneDouble;

        std::memcpy(&memory[flagSize], &bufferSize, bufferSizeSize);
        
        // 3) copy double* into memory buffer
        std::memcpy(&memory[flagSize + bufferSizeSize], data, bufferSize);
    }



    inline void destroy() {
        _memory.destroy();
    }

private:
    Memory _memory;
    Error memoryErrorStetus = kOK;
};


}; // namespace lsm

#endif // INCLUDE_LIBSHAREDMEMORY_HPP_