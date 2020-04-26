//
// Class for manipulations with filesystem object (file)
//

#ifndef CPP_HASHCACHE_SNAPSHOT_F_HPP_
#define CPP_HASHCACHE_SNAPSHOT_F_HPP_

#include <filesystem>

namespace fs = std::filesystem;

namespace hashcache {

class file_info {
    public:
        file_info( fs::path const& file_name );

        const std::string& get_digest();
        int64_t get_ts() const { return m_mtime; };
        fs::path const& get_filename() const { return m_filename; };

        // Better to be implemented in more cross-platform way (see current implementation in file_info.cpp)
        static void last_write_time( fs::path const& file_name, int64_t new_time );
        static int64_t last_write_time( fs::path const& file_name );


    private:
        void _create_digest();
        static void _generate_digest_byte_array( fs::path const& file_name, unsigned char* digest_buf );

        int64_t m_mtime;
        fs::path m_filename;
        std::string m_digest;

};

}

#endif
