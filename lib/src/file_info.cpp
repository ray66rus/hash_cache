#include <cerrno>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <openssl/md5.h>

#include <hash_snapshot/exception.hpp>
#include <hash_snapshot/file_info.hpp>

#define STREAM_BUFFSIZE 16000

#define __HASH_SNAPSHOT_IO_EXCEPTION( MSG, FNAME )    hash_snapshot_exception( \
            std::string( MSG ) + " (" + FNAME + ")", \
            hash_snapshot_exception::error_type::IO_ERROR )

namespace hashcache {

file_info::file_info( fs::path const& file_name )
{
    try {
        m_digest = "";
        m_filename = file_name;
        m_ts = fs::last_write_time( file_name );

    } catch( const fs::filesystem_error& e ) {
        throw __HASH_SNAPSHOT_IO_EXCEPTION( e.what(), file_name.string() );
    } catch ( const std::ifstream::failure& e ) {
        throw __HASH_SNAPSHOT_IO_EXCEPTION( e.what(), file_name.string() );
    }
}

const std::string& file_info::get_digest()
{
    if( m_digest == "" ) {
        _create_digest();
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
    char buffer[ STREAM_BUFFSIZE ] = { 0 };
    std::ifstream ifs;
    MD5_CTX md5Context;

    ifs.exceptions( std::ifstream::badbit );
    ifs.open( file_name, std::ifstream::binary );

    if ( !ifs.good() ) {
        throw __HASH_SNAPSHOT_IO_EXCEPTION( "Can't open file ", file_name.string() );
    }

    // Calculate digest
    MD5_Init( &md5Context );

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

}
