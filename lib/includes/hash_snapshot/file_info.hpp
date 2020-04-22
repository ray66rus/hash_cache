#ifndef CPP_HASHCACHE_SNAPSHOT_F_HPP_
#define CPP_HASHCACHE_SNAPSHOT_F_HPP_

#include <ctime>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace hashcache {

class file_info {
    public:
        file_info( fs::path const& file_name );

        const std::string& get_digest();
        std::time_t get_ts() const { return m_ts; };
        fs::path const& get_filename() const { return m_filename; };

    private:
        void _create_digest();
        static void _generate_digest_byte_array( fs::path const& file_name, unsigned char* digest_buf );

        std::time_t m_ts;
        fs::path m_filename;
        std::string m_digest;

};

}

#endif
