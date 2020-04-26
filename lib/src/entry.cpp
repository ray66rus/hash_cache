#include <array>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <chrono>

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/entry.hpp>

#define MICROSECONDS_LENGTH 6
#define MICROSECONDS_LENGTH 6

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

    if( m_digest == "" || m_filename == "" || _is_datetime_format_bad( date, time ) ) {
        throw hash_snapshot_exception( "Invalid entry in snapshot file: \"" + line + "\"",
            hash_snapshot_exception::error_type::PARSE_ERROR );
    }

    m_mtime = _mtime_from_string( date + " " + time );
}

bool shapshot_entry_string::_is_datetime_format_bad( std::string const& date, std::string const& time )
{
    if( date.find_first_not_of( "0123456789-" ) != std::string::npos ) {
        return true;
    }

    if( time.find_first_not_of( "0123456789:." ) != std::string::npos ) {
        return true;
    }

    size_t point_position = time.find( "." );
    if( point_position == std::string::npos || time.length() - point_position != MICROSECONDS_LENGTH + 1 ) {
        return true;
    }

    return false;
}

int64_t shapshot_entry_string::_mtime_from_string( const std::string& ts_str )
{
    std::stringstream ss( ts_str );
    std::tm tm {};
    tm.tm_isdst = -1;

    ss >> std::get_time( &tm, "%Y-%m-%d %H:%M:%S." );
    auto ts_timepoint = chrono::system_clock::from_time_t( std::mktime( &tm ) );

    int64_t msec_int;
    ss >> msec_int;
    chrono::microseconds msec{ msec_int };

    auto ts = chrono::duration_cast<chrono::microseconds>( ts_timepoint.time_since_epoch() ) + msec;
    return ts.count();
}

shapshot_entry_string::shapshot_entry_string( const std::string& file_name, const snapshot_entry& entry )
{
    m_mtime = entry.mtime;
    m_digest = entry.digest;
    m_filename = file_name;
}

shapshot_entry_string::operator std::string() const
{
    chrono::microseconds msec{ m_mtime };
    chrono::seconds sec = chrono::duration_cast<chrono::seconds>( msec );
    chrono::time_point<chrono::system_clock> ts_timepoint( sec );
    std::time_t ts_timet = chrono::system_clock::to_time_t( ts_timepoint );

    std::stringstream res;
    res << std::put_time( std::localtime( &ts_timet ), "%Y-%m-%d %H:%M:%S." )
            << std::setfill( '0' ) << std::setw( MICROSECONDS_LENGTH ) << ( msec - sec ).count()
            << " " << m_digest
            << " " << m_filename;

    return res.str();
}

}
