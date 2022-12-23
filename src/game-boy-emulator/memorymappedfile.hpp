#pragma once

#include <filesystem>
#include <span>
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
    MemoryMappedFile( MemoryMappedFile&&) = delete;
    MemoryMappedFile& operator=(MemoryMappedFile&&) = delete;
};
