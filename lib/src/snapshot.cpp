#include <algorithm>
#include <iterator>

#include <hash_snapshot/hash_snapshot.hpp>

#include <iostream>

#ifdef DEBUG_ON
#define __DEBUG_MSG( STR )   std::cerr << __FUNCTION__ << ": " << ( STR ) << std::endl
#else
#define __DEBUG_MSG( STR )
#endif


namespace hashcache {

snapshot::snapshot( std::string const& file_name, std::string const& dir, bool follow_symlinks )
{
    m_follow_symlinks = follow_symlinks;
    m_last_scan_ts = 0;

    m_working_dir = dir;

    m_storage.init( _full_file_name( file_name ) );
}

void snapshot::scan_working_dir()
{
    m_storage.load();

    m_last_scan_ts = _HASH_SNAPSHOT_TIMET_NOW;

    fs::recursive_directory_iterator begin( m_working_dir,
            ( m_follow_symlinks ? fs::directory_options::follow_directory_symlink : fs::directory_options::none ) |
            fs::directory_options::skip_permission_denied );

    fs::recursive_directory_iterator end;

    for( auto i = begin; i != end; ++i ) {
        _process_filesystem_object( i ); // side-effect: changes parameter, prohibits recursion to dirs starting with "."
    }
}

void snapshot::_process_filesystem_object( fs::recursive_directory_iterator& i )
{
    auto const& path = i->path();
    std::error_code ec;

    try {
        if ( path.filename().string()[ 0 ] == '.' ) {
            i.disable_recursion_pending();
            return;
        }

        if( fs::is_directory( path, ec ) ) {
            return;
        } else if( ec ) {
            std::cerr << "Can't check data for path " << path << ": " << ec.message() << std::endl;
            return;
        }

        _update_meta_or_create_entry( path );

    } catch( hash_snapshot_exception const& e ) {
        std::cerr << "Can't get info for file " << path << ". " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Can't process filesystem path for unknown reason" << std::endl;
        return;
    }
}

void snapshot::_update_meta_or_create_entry( fs::path const& path )
{
    file_info info( path );
    auto storage_key = path.lexically_relative( m_working_dir ).string();

    try {
        m_storage.update_entry_metadata( storage_key, info );
    } catch( std::out_of_range ) {
        m_storage.add_entry( storage_key, info );
    }
}

std::vector<std::string> snapshot::modified_files_list()
{
    if( m_last_scan_ts == 0 ) {
        std::cerr << "Working directory wasn't scanned, empty list return" << std::endl;
        return {};
    }

    return m_storage.get_changed_mtime_entries_list();
}

bool snapshot::is_file_contents_changed( const std::string& file_name )
{
    file_info info( _full_file_name( file_name ) );
    return is_file_contents_changed( file_name, info );
}

bool snapshot::is_file_contents_changed( const std::string& file_name, file_info& file_info )
{
    bool res = false;

    try {
        res = m_storage.is_entry_digest_differs( file_name, file_info.get_digest() );
    } catch( std::out_of_range ) {
        __DEBUG_MSG( "entry for file name \"" +  file_name + "\" not found!" );
    } catch ( hash_snapshot_exception const& e ) {
        __DEBUG_MSG( "file contents check failed. " + std::string( e.what() ) );
    }

    return res;
}

void snapshot::revert_file_mtime( const std::string& file_name )
{
    fs::path full_file_name = _full_file_name( file_name );

    try {
        std::int64_t ts = m_storage.get_entry_mtime( file_name );
        file_info::last_write_time( full_file_name, ts );

        file_info info( full_file_name );
        m_storage.update_entry_metadata( file_name, info );

    } catch( std::out_of_range ) {
        __DEBUG_MSG( "entry for file name \"" + file_name + "\" not found!" );
    } catch ( fs::filesystem_error const& e ) {
        __DEBUG_MSG( "file modification time update failed. " + std::string( e.what() ) );
    }
}

void snapshot::update_file_digest( const std::string &file_name  )
{
    file_info info( _full_file_name( file_name ) );
    update_file_digest( file_name, info );
}

void snapshot::update_file_digest( const std::string &file_name, file_info& file_info  )
{
    try {
        m_storage.update_entry( file_name, file_info );
    } catch( std::out_of_range ) {
        __DEBUG_MSG( "entry for file name \"" + file_name + "\" not found!" );
    } catch ( hash_snapshot_exception const& e ) {
        __DEBUG_MSG( "file contents check failed. " + std::string( e.what() ) );
    }
}

void snapshot::fix_file( const std::string& file_name ) // Create file_info object once for both check and fix ops (if fix is needed)
{
    file_info info( _full_file_name( file_name ) );

    if( is_file_contents_changed( file_name, info ) ) {
        update_file_digest( file_name, info );
    } else {
        revert_file_mtime( file_name );
    }
}

void snapshot::save( void )
{
    m_storage.save( m_last_scan_ts );  // Don't save files that were deleted before the last scan
}

}
