//
// Classes for snapshot file and inner storage structure entries
//

#ifndef CPP_HASHCACHE_SNAPSHOT_ENTRY_HPP_
#define CPP_HASHCACHE_SNAPSHOT_ENTRY_HPP_

#include <ctime>
#include <string>

namespace hashcache {

struct snapshot_entry;


// Serialize/deserialize internal storage unit to/from string
class shapshot_entry_string {
    public:
        shapshot_entry_string( const std::string& line );
        shapshot_entry_string( const std::string& file_name, const snapshot_entry& entry );

        int64_t get_mtime() const { return m_mtime; }
        const std::string& get_digest() const { return m_digest; }
        const std::string& get_filename() const { return m_filename; }

        operator std::string() const;

    private:
        static int64_t _mtime_from_string( const std::string& ts_str );
        static bool _is_datetime_format_bad( std::string const& date, std::string const& time );

        int64_t m_mtime;
        std::string m_digest;
        std::string m_filename;
};

// Internal storage unit
struct snapshot_entry {
    snapshot_entry(): mtime( 0 ), last_seen( 0 ), mtime_changed ( false ) {};

    snapshot_entry( const std::string& digest, int64_t ts, std::time_t last_seen ):
        digest( digest ), mtime( ts ), last_seen( last_seen ), mtime_changed( false ) {};

    snapshot_entry( const shapshot_entry_string& e_str ):
        snapshot_entry( e_str.get_digest(), e_str.get_mtime(), 0 ) {};

    // Data, discribing the file - saved in snapshot file
    std::string digest;
    int64_t mtime;

    // Metadata - not saved in snapshot file, updates by the storage class methods
    std::time_t last_seen; // Timestamp when file corresponding to this entry was seen on disk. Used for removing entries for deleted files
    bool mtime_changed; // Was the modification time for the file changed?

};

}

#endif
