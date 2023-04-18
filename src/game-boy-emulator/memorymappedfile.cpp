#include "memorymappedfile.hpp"
#include "io.hpp"

#ifndef __EMSCRIPTEN__

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& filepath, size_t file_size) :
        m_file_size(file_size) {

    // Create file if it does not exist
    if (!exists(filepath)) {
        EmulatorIo::create_file(filepath, m_file_size);
    }

    m_file = boost::interprocess::file_mapping{filepath.string().c_str(),
                                               boost::interprocess::read_write};
    m_mapped_region = boost::interprocess::mapped_region{m_file, boost::interprocess::read_write, 0,
                                                         m_file_size};
}

std::span<uint8_t> MemoryMappedFile::get_data() {
    return {std::bit_cast<uint8_t*>(m_mapped_region.get_address()), m_file_size};
}

void MemoryMappedFile::sync(bool async) {
    m_mapped_region.flush(0, 0, async);
}

MemoryMappedFile::~MemoryMappedFile() {
    m_mapped_region.flush(0, 0, true);
}

#endif
