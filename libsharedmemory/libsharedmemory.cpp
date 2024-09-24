#include "libsharedmemory.hpp"

namespace lsm {

// Windows shared memory implementation
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <io.h>  // CreateFileMappingA, OpenFileMappingA, etc.

Memory::Memory(const std::string path, const std::size_t size, const bool persist) : _path(path), _size(size), _persist(persist) {};

Error Memory::createOrOpen(const bool create) {
    if (create) {
        DWORD size_high_order = 0;
        DWORD size_low_order = static_cast<DWORD>(_size);

        _handle = CreateFileMappingA(INVALID_HANDLE_VALUE,  // use paging file
                                        NULL,                  // default security
                                        PAGE_READWRITE,        // read/write access
                                        size_high_order, size_low_order,
                                        _path.c_str()  // name of mapping object
        );

        if (!_handle) {
            return kErrorCreationFailed;
        }
    } else {
      _handle = OpenFileMappingA(FILE_MAP_READ, // read access
                                 FALSE,         // do not inherit the name
                                 _path.c_str()  // name of mapping object
      );

      // TODO: Windows has no default support for shared memory persistence
      // see: destroy() to implement that

        if (!_handle) {
            return kErrorOpeningFailed;
        }
    }

    // TODO: might want to use GetWriteWatch to get called whenever
    // the memory section changes
    // https://docs.microsoft.com/de-de/windows/win32/api/memoryapi/nf-memoryapi-getwritewatch?redirectedfrom=MSDN

    const DWORD access = create ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
    _data = MapViewOfFile(_handle, access, 0, 0, _size);

    if (!_data) {
        return kErrorMappingFailed;
    }
    return kOK;
}

void Memory::destroy() {

  // TODO: Windows needs priviledges to define a shared memory (file mapping)
  // OBJ_PERMANENT; furthermore, ZwCreateSection would need to be used.
  // Instead of doing this; saving a file here (by name, temp dir)
  // and reading memory from file in createOrOpen seems more suitable.
  // Especially, because files can be removed on reboot using:
  // MoveFileEx() with the MOVEFILE_DELAY_UNTIL_REBOOT flag and lpNewFileName
  // set to NULL.
}

void Memory::close() {
    if (_data) {
        UnmapViewOfFile(_data);
        _data = nullptr;
    }
    CloseHandle(_handle);
}

Memory::~Memory() {
    close();
    if (!_persist) {
      destroy();
    }
}
#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)


#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION) || defined(__ANDROID__)

#include <fcntl.h>     // for O_* constants
#include <sys/mman.h>  // mmap, munmap
#include <sys/stat.h>  // for mode constants
#include <unistd.h>    // unlink

#if defined(__APPLE__)

#include <errno.h>

#endif // __APPLE__

#include <stdexcept>

inline Memory::Memory(const std::string path, const std::size_t size, const bool persist) : _size(size), _persist(persist) {
    _path = "/" + path;
};

inline Error Memory::createOrOpen(const bool create) {
    if (create) {
        // shm segments persist across runs, and macOS will refuse
        // to ftruncate an existing shm segment, so to be on the safe
        // side, we unlink it beforehand.
        const int ret = shm_unlink(_path.c_str());
        if (ret < 0) {
            if (errno != ENOENT) {
                return kErrorCreationFailed;
            }
        }
    }

    const int flags = create ? (O_CREAT | O_RDWR) : O_RDONLY;

    _fd = shm_open(_path.c_str(), flags, 0755);
    if (_fd < 0) {
        if (create) {
            return kErrorCreationFailed;
        } else {
            return kErrorOpeningFailed;
        }
    }

    if (create) {
        // this is the only way to specify the size of a
        // newly-created POSIX shared memory object
        int ret = ftruncate(_fd, _size);
        if (ret != 0) {
            return kErrorCreationFailed;
        }
    }

    const int prot = create ? (PROT_READ | PROT_WRITE) : PROT_READ;

    _data = mmap(nullptr,    // addr
                 _size,      // length
                 prot,       // prot
                 MAP_SHARED, // flags
                 _fd,        // fd
                 0           // offset
    );

    if (_data == MAP_FAILED) {
        return kErrorMappingFailed;
    }

    if (!_data) {
        return kErrorMappingFailed;
    }
    return kOK;
}

inline void Memory::destroy() { shm_unlink(_path.c_str()); }

inline void Memory::close() {
  munmap(_data, _size);
  ::close(_fd);
}

inline Memory::~Memory() {
    close();
    if (!_persist) {
        destroy();
    }
}

#endif // defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION) || defined(__ANDROID__)

}