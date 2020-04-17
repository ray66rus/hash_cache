#include <array>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <chrono>

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/entry.hpp>
#include <iostream>
namespace chrono = std::chrono;

namespace hashcache {

shapshot_entry_string::shapshot_entry_string( const std::string& line )
{
    std::istringstream iss( line );
    std::string date, time;

    std::getline( iss, date, ' ' );
    std::getline( iss, time, ' ' );
    std::getline( iss, m_digest, ' ' );
    std::getline( iss, m_filename );

    if( m_digest == "" ||
        m_filename == "" ||
        date.find_first_not_of( "0123456789-" ) != std::string::npos ||
        time.find_first_not_of( "0123456789:." ) != std::string::npos ) { // simple format check

        throw hash_snapshot_exception( "Invalid entry in snapshot file: \"" + line + "\"",
            hash_snapshot_exception::error_type::PARSE_ERROR );
    }

    m_ts = _ts_from_string( date + " " + time );
}

// isn't this function a little bit overengeneered for a compatability sake?
uint64_t shapshot_entry_string::_ts_from_string( const std::string& ts_str )
{
    std::stringstream ss( ts_str );
    std::tm tm {};
    uint64_t nsec_int;

    ss >> std::get_time( &tm, "%Y-%m-%d %H:%M:%S." );
    auto ts_timepoint = chrono::system_clock::from_time_t( std::mktime( &tm ) );

    ss >> nsec_int;
    chrono::nanoseconds nsec{ nsec_int };

    auto ts = chrono::duration_cast<chrono::nanoseconds>( ts_timepoint.time_since_epoch() ) + nsec;

    return ts.count();
}

shapshot_entry_string::shapshot_entry_string( const std::string& file_name, const snapshot_entry& entry )
{
    m_ts = entry.ts;
    m_digest = entry.digest;
    m_filename = file_name;
}

shapshot_entry_string::operator std::string() const
{
    chrono::nanoseconds nsec{ m_ts };
    chrono::seconds sec = chrono::duration_cast<chrono::seconds>( nsec );
    chrono::time_point<std::chrono::system_clock> ts_timepoint( sec );
    std::time_t ts_timet = std::chrono::system_clock::to_time_t( ts_timepoint );

    std::stringstream ts_stream;
    ts_stream << std::put_time( std::localtime( &ts_timet ), "%Y-%m-%d %H:%M:%S." )
            << std::setfill( '0' ) << std::setw( 9 ) << ( nsec - sec ).count();

    return ts_stream.str() + " " + m_digest + " " + m_filename;
}

}
