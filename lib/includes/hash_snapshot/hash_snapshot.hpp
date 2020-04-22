#ifndef CPP_HASHCACHE_STORAGE_HPP_
#define CPP_HASHCACHE_STORAGE_HPP_

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/storage.hpp>

#include <hash_snapshot_export.h>

namespace fs = std::filesystem;

namespace hashcache {

class snapshot {
    public:
        snapshot( const fs::path& file_name ): snapshot( file_name, fs::path("."), false ) {};
        HASH_SNAPSHOT_EXPORT snapshot( const fs::path& file_name, const fs::path& dir, bool follow_symlinks = false );

        HASH_SNAPSHOT_EXPORT void scan_working_dir();

        HASH_SNAPSHOT_EXPORT std::vector<std::string> modified_files_list();

        HASH_SNAPSHOT_EXPORT bool is_file_contents_changed( const std::string& file_name );
        HASH_SNAPSHOT_EXPORT void revert_file_ts( const std::string& file_name );
        HASH_SNAPSHOT_EXPORT void update_file_digest( const std::string& file_name );

        HASH_SNAPSHOT_EXPORT void fix_file( const std::string& file_name );

        HASH_SNAPSHOT_EXPORT void save( void );

    private:
        bool is_file_contents_changed( const std::string& file_name, file_info& file_info );
        void update_file_digest( const std::string& file_name, file_info& file_info );

        void _update_meta_or_create_entry( fs::path const& path );
        fs::path const _full_file_name( fs::path const& file_name ) { return m_working_dir / file_name; }

        snapshot_storage m_storage;

        fs::path m_working_dir;
        bool m_follow_symlinks;

        bool m_is_loaded = false;

        std::time_t m_last_scan_ts; // Timestamp of last working directory scan start
};

}

#endif
