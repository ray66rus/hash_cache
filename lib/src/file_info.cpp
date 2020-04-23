#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>

#include <openssl/md5.h>

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/file_info.hpp>

#ifdef _WIN32   // Uh-huh....
#include <windows.h>
#include <fileapi.h>

#define MICROSECONDS_EPOCH_SHIFT    11644473600000000
#define WINTIME_MICROSECONDS_FACTOR 10
#endif

namespace chrono = std::chrono;

#define STREAM_BUFFSIZE 16000

namespace hashcache {

file_info::file_info( fs::path const& file_name )
{
    try {
        m_digest = "";
        m_filename = file_name;
        m_ts = last_write_time( file_name );
    } catch( fs::filesystem_error const &e ) {
        throw hash_snapshot_exception( e.what(), hash_snapshot_exception::error_type::IO_ERROR );
    }
}

const std::string& file_info::get_digest()
{
    if( m_digest != "" ) {
        return m_digest;
    }

    try {
        _create_digest();
    } catch ( hash_snapshot_exception ) {
        throw;
    } catch( std::exception const& e ) {
        throw hash_snapshot_exception( std::string( "Unknown error while creating the file digest: " ) + e.what(),
            hash_snapshot_exception::error_type::UNKNWON_ERROR );
    }

    return m_digest;
}

void file_info::_create_digest()
{
    unsigned char digest[ MD5_DIGEST_LENGTH ] = { 0 };
    std::stringstream ss;

    _generate_digest_byte_array( m_filename, digest );

    // Convert digest to string
    ss << std::hex << std::setfill( '0' );
    for( unsigned char uc: digest ) {
        ss << std::setw( 2 ) << ( int ) uc;
    }

    // Copy digest string to class member
    m_digest = ss.str();
}

void file_info::_generate_digest_byte_array( fs::path const& file_name, unsigned char* digest_buf )
{
    MD5_CTX md5Context;
    MD5_Init( &md5Context );

    std::ifstream ifs( fs::canonical( file_name ).string(), std::ifstream::binary );

    char buffer[ STREAM_BUFFSIZE ] = { 0 };
    while( ifs.good() ) {
        ifs.read( buffer, STREAM_BUFFSIZE );

        MD5_Update( &md5Context, buffer, ifs.gcount() );
    }
    ifs.close();

    if( MD5_Final( digest_buf, &md5Context ) == 0 ) {
        throw hash_snapshot_exception( "Can't calculate digest for the file " + file_name.string(),
            hash_snapshot_exception::error_type::DIGEST_ERROR );
    }

}

#ifdef _WIN32
int64_t inline win_filetime_to_ms_from_epoch(FILETIME& ft) {
    ULARGE_INTEGER ft_64;

    ft_64.LowPart = ft.dwLowDateTime;
    ft_64.HighPart = ft.dwHighDateTime;

    return static_cast<int64_t>(ft_64.QuadPart / WINTIME_MICROSECONDS_FACTOR - MICROSECONDS_EPOCH_SHIFT);
}

void inline ms_from_epoch_to_win_filetime(int64_t ms_from_epoch, FILETIME& ft)
{
    ULARGE_INTEGER ft_64;
    ft_64.QuadPart = (ms_from_epoch + MICROSECONDS_EPOCH_SHIFT ) * WINTIME_MICROSECONDS_FACTOR;

    ft.dwLowDateTime = ft_64.LowPart;
    ft.dwHighDateTime = ft_64.HighPart;
}
#endif

int64_t file_info::last_write_time( fs::path const& file_name )
{
#ifdef _WIN32

    WIN32_FILE_ATTRIBUTE_DATA raw_info;

    if( !::GetFileAttributesExA( file_name.string().c_str(), GetFileExInfoStandard, &raw_info ) ) {
        auto ec = std::error_code( ::GetLastError(), std::system_category() );
        throw fs::filesystem_error( "can't get file attributes", file_name.string(), ec );
    }

    return win_filetime_to_ms_from_epoch(raw_info.ftLastWriteTime);

#else

    chrono::microseconds msec = chrono::duration_cast<chrono::microseconds>( fs::last_write_time( file_name ).time_since_epoch() );
    return msec.count();

#endif
    return 0;
}

void file_info::last_write_time(fs::path const& file_name, int64_t new_time)
{
#ifdef _WIN32

    HANDLE file_h = ::CreateFile(file_name.string().c_str(),
        FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if( file_h == INVALID_HANDLE_VALUE ) {
        auto ec = std::error_code( ::GetLastError(), std::system_category() );
        throw fs::filesystem_error( "can't open file for setting time", file_name.string(), ec );
    }

    FILETIME ft;
    ms_from_epoch_to_win_filetime(new_time, ft);

    if( !::SetFileTime(file_h, (LPFILETIME)NULL, (LPFILETIME)NULL, &ft ) ) {
        CloseHandle(file_h);
        auto ec = std::error_code( ::GetLastError(), std::system_category() );
        throw fs::filesystem_error( "can't open file for setting time", file_name.string(), ec );
    }

    CloseHandle(file_h);

#else

    std::chrono::microseconds msec{ new_time };
    fs::last_write_time( file_name, chrono::time_point<chrono::system_clock>( msec ) );

#endif
}


}
