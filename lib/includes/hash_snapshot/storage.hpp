#ifndef CPP_HASHCACHE_SNAPSHOT_HPP_
#define CPP_HASHCACHE_SNAPSHOT_HPP_

#include <string>
#include <map>
#include <chrono>
#include <filesystem>
//
// Snapshot structure data and operations under this data
//

#include <hash_snapshot/entry.hpp>
#include <hash_snapshot/file_info.hpp>

#define _HASH_SNAPSHOT_TIMET_NOW   ( std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() ) )

namespace hashcache {

class snapshot_storage {
    public:
        snapshot_storage(): m_filename( "" ) {};

        void init( fs::path const& file_name ); // Try to create snapshot file and throws exception if can't find or create it

        void load();
        void save( std::time_t min_last_seen = 0 ); // Doesn't save entries which last_seen field is less than given as a parameter

        bool empty() const { return m_storage.empty(); };
        std::size_t size() const { return m_storage.size(); };

        void add_entry( const std::string& file_name, file_info& data );
        void remove_entry( const std::string& file_name );
        void update_entry( const std::string& file_name, file_info& data ) { add_entry( file_name, data ); }
        void update_entry_metadata( const std::string& file_name, const file_info& data ); // updates meta data for entry - is mtime changed and "last seen field"

        std::vector<std::string> const get_changed_mtime_entries_list();
        bool is_entry_digest_differs( const std::string& file_name, const std::string& digest );
        int64_t get_entry_mtime( const std::string& file_name );

    private:
        void _create_snapshot_file();

        void add_entry( const std::string& line );
        void add_entry( const std::string& file_name, const snapshot_entry& entry ) { m_storage[ file_name ] = entry; }

        fs::path m_filename;
        std::map<std::string, hashcache::snapshot_entry> m_storage;
};

}

#endif
