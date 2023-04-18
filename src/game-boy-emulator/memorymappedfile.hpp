#pragma once

#include <filesystem>
#include <span>


#ifndef __EMSCRIPTEN__

#include "boost/interprocess/file_mapping.hpp"
#include "boost/interprocess/mapped_region.hpp"


class MemoryMappedFile {
    size_t m_file_size;
    boost::interprocess::file_mapping m_file;
    boost::interprocess::mapped_region m_mapped_region;

public:
    MemoryMappedFile(const std::filesystem::path& filepath, size_t file_size);
    std::span<uint8_t> get_data();
    void sync(bool async = false);

    ~MemoryMappedFile();

    MemoryMappedFile(const MemoryMappedFile&) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
    MemoryMappedFile(MemoryMappedFile&&) = delete;
    MemoryMappedFile& operator=(MemoryMappedFile&&) = delete;
};

#else

class MemoryMappedFile {
    size_t m_file_size;
    std::vector<uint8_t> m_file;
public:

    MemoryMappedFile(const std::filesystem::path& filepath, size_t file_size) : m_file_size(file_size), m_file(file_size, 0) {
    }
    std::span<uint8_t> get_data() {
        return m_file;
    }
    void sync(bool async = false){}

    ~MemoryMappedFile() = default;

    MemoryMappedFile(const MemoryMappedFile&) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
    MemoryMappedFile(MemoryMappedFile&&) = delete;
    MemoryMappedFile& operator=(MemoryMappedFile&&) = delete;
};


#endif