#ifndef CPP_HASHCACHE_SNAPSHOT_HPP_
#define CPP_HASHCACHE_SNAPSHOT_HPP_

#include <string>
#include <map>
#include <chrono>
#include <filesystem>

#include <hash_snapshot/entry.hpp>
#include <hash_snapshot/file_info.hpp>

#define _HASH_SNAPSHOT_TIMET_NOW   ( std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() ) )

namespace hashcache {

class snapshot_storage {
    public:
        snapshot_storage(): m_filename( "" ) {};

        void init( fs::path const& file_name );

        void load();
        void save( std::time_t min_last_seen = 0 );

        bool empty() const { return m_storage.empty(); };
        std::size_t size() const { return m_storage.size(); };

        void add_entry( const std::string& file_name, file_info& data );
        void remove_entry( const std::string& file_name );

        std::vector<std::string> const get_ts_changed_files_list();
        bool is_file_contens_changed( const std::string& file_name, const std::string& digest );
        std::time_t get_saved_file_ts( const std::string& file_name );

        void update_entry( const std::string& file_name, file_info& data ) { add_entry( file_name, data ); }
        void update_entry_metadata( const std::string& file_name, const file_info& data );

    private:
        void _create_snapshot_file();

        void add_entry( const std::string& line );
        void add_entry( const std::string& file_name, const snapshot_entry& entry ) { m_storage[ file_name ] = entry; }

        fs::path m_filename;
        std::map<std::string, hashcache::snapshot_entry> m_storage;
};

}

#endif
