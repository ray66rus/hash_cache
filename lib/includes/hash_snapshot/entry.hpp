#ifndef CPP_HASHCACHE_SNAPSHOT_ENTRY_HPP_
#define CPP_HASHCACHE_SNAPSHOT_ENTRY_HPP_

#include <string>
#include <chrono>

namespace hashcache {

struct snapshot_entry;

class shapshot_entry_string {
    public:
        shapshot_entry_string( const std::string& line );
        shapshot_entry_string( const std::string& file_name, const snapshot_entry& entry );

        uint64_t get_ts() const { return m_ts; }
        const std::string& get_digest() const { return m_digest; }
        const std::string& get_filename() const { return m_filename; }

        operator std::string() const;

    private:
        static uint64_t _ts_from_string( const std::string& ts_str );

        uint64_t m_ts;
        std::string m_digest;
        std::string m_filename;
};

struct snapshot_entry {
    snapshot_entry(): ts( 0 ), last_seen( 0 ), ts_changed ( false ) {};

    snapshot_entry( const std::string& digest, uint64_t ts, std::time_t last_seen ):
        digest( digest ), ts( ts ), last_seen( last_seen ), ts_changed( false ) {};

    snapshot_entry( const shapshot_entry_string& e_str ):
        snapshot_entry( e_str.get_digest(), e_str.get_ts(), 0 ) {};

    std::string digest;
    uint64_t ts;

    std::time_t last_seen;
    bool ts_changed;

};

}

#endif
