#include <fstream>
#include <cstring>
#include <chrono>
#include <iostream>
#include <algorithm>

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/storage.hpp>

namespace fs = std::filesystem;

namespace hashcache {

void snapshot_storage::init( fs::path const& file_name )
{
    m_filename = file_name;

    if ( !fs::is_regular_file( m_filename ) ) {
        _create_snapshot_file();
    }
}

void snapshot_storage::_create_snapshot_file()
{
    std::ofstream ofs;

    ofs.exceptions( std::ios_base::failbit | std::ios_base::badbit );
    ofs.open( m_filename.string() );
    ofs.close();
}

void snapshot_storage::load()
{
    std::string line;
    std::ifstream ifs( fs::canonical( m_filename ).string() );

    while( ifs.peek() != EOF ) {
        std::getline( ifs, line );
        add_entry( line );
    }
    ifs.close();
}

void snapshot_storage::add_entry( const std::string& line )
{
    try {
        const shapshot_entry_string entry_string( line );
        snapshot_entry entry( entry_string );

        add_entry( entry_string.get_filename(), entry );
    } catch( const hash_snapshot_exception& e ) {
        std::cerr << "Error adding snapshot entry. " << e.what() << std::endl;
    }
}

void snapshot_storage::remove_entry( const std::string& file_name )
{
    m_storage.erase( file_name );
}

void snapshot_storage::update_entry_metadata( const std::string &file_name, const file_info& data )
{
    auto& entry = m_storage.at( file_name );
    entry.last_seen = _HASH_SNAPSHOT_TIMET_NOW;
    entry.mtime_changed = ( entry.mtime != data.get_ts() );
}

std::vector<std::string> const snapshot_storage::get_changed_mtime_entries_list()
{
    std::vector<std::string> res;

    std::for_each( m_storage.begin(), m_storage.end(), [ &res ]( auto const& it ) {
        if( it.second.mtime_changed ) res.push_back( it.first );
    } );

    return res;
}

bool snapshot_storage::is_entry_digest_differs( const std::string& file_name, const std::string& digest )
{
    auto const& entry = m_storage.at( file_name );
    return entry.digest != digest;
}

void snapshot_storage::save( std::time_t min_last_seen )
{
    std::ofstream ofs;

    ofs.exceptions( std::ofstream::badbit | std::ofstream::failbit );
    ofs.open( m_filename.string(), std::ios_base::trunc );

    for( auto const& it: m_storage ) {
        auto const& file_name = it.first;
        auto const& entry = it.second;

        if( entry.last_seen >= min_last_seen ) {
            ofs << std::string( shapshot_entry_string( it.first, it.second ) ) << std::endl;
        }
    }

    ofs.close();
}

void snapshot_storage::add_entry( const std::string& file_name, file_info& data )
{
    const snapshot_entry entry( data.get_digest(), data.get_ts(), _HASH_SNAPSHOT_TIMET_NOW );
    add_entry( file_name, entry );
}

int64_t snapshot_storage::get_entry_mtime( const std::string& file_name )
{
    auto const& entry = m_storage.at( file_name );
    return entry.mtime;
}

}
