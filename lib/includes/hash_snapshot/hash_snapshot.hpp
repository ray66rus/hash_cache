//
// Main hash_snapshot library class
//

#ifndef CPP_HASHCACHE_STORAGE_HPP_
#define CPP_HASHCACHE_STORAGE_HPP_

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/storage.hpp>

#include <hash_snapshot_export.h>

namespace hashcache {

class snapshot {
    public:
        snapshot( std::string const& file_name ): snapshot( file_name, ".", false ) {};
        HASH_SNAPSHOT_EXPORT snapshot( std::string const& file_name, std::string const& dir, bool follow_symlinks = false );

        HASH_SNAPSHOT_EXPORT void scan_working_dir();

        HASH_SNAPSHOT_EXPORT std::vector<std::string> modified_files_list();

        HASH_SNAPSHOT_EXPORT bool is_file_contents_changed( const std::string& file_name );
        HASH_SNAPSHOT_EXPORT void revert_file_mtime( const std::string& file_name );
        HASH_SNAPSHOT_EXPORT void update_file_digest( const std::string& file_name );

        HASH_SNAPSHOT_EXPORT void fix_file( const std::string& file_name ); // "Shortcut" for the most likely use case

        HASH_SNAPSHOT_EXPORT void save( void );

    private:
        bool is_file_contents_changed( const std::string& file_name, file_info& file_info );
        void update_file_digest( const std::string& file_name, file_info& file_info );

        void _process_filesystem_object( fs::recursive_directory_iterator& i );
        void _update_meta_or_create_entry( fs::path const& path );
        fs::path const _full_file_name( fs::path const& file_name ) { return m_working_dir / file_name; }

        snapshot_storage m_storage; // Snapshot structure and its operations

        fs::path m_working_dir;
        bool m_follow_symlinks;

        bool m_is_loaded = false;

        std::time_t m_last_scan_ts; // Timestamp of last working directory scan start
};

}

#endif
