#include <array>
#include <sstream>
#include <iterator>
#include <iomanip>

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/entry.hpp>

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

std::time_t shapshot_entry_string::_ts_from_string( const std::string& ts_str )
{
    std::stringstream ss( ts_str );

    std::tm tm {};
    ss >> std::get_time( &tm, "%Y-%m-%d %H:%M:%S" );

    return std::mktime( &tm );
}

shapshot_entry_string::shapshot_entry_string( const std::string& file_name, const snapshot_entry& entry )
{
    m_ts = entry.ts;
    m_digest = entry.digest;
    m_filename = file_name;
}

shapshot_entry_string::operator std::string() const
{
    std::stringstream ts_stream;

    ts_stream << std::put_time( std::localtime( &m_ts ), "%Y-%m-%d %H:%M:%S" );
    return ts_stream.str() + " " + m_digest + " " + m_filename;
}

}
