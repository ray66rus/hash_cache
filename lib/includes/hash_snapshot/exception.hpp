#ifndef CPP_HASHCACHE_SNAPSHOT_EXC_HPP_
#define CPP_HASHCACHE_SNAPSHOT_EXC_HPP_

#include <string>
#include <exception>

#include <hash_snapshot_export.h>

namespace hashcache {

class hash_snapshot_exception: public std::exception {
    public:
        enum class error_type { IO_ERROR, PARSE_ERROR, DIGEST_ERROR };

        explicit hash_snapshot_exception( const std::string& msg, error_type type ):
            m_error_message( msg ), m_error_type( type ) {};

        const char* what() const noexcept { return m_error_message.c_str(); };
        error_type type() const { return m_error_type; };

    private:
        const std::string m_error_message;
        const error_type m_error_type;
};

}

#endif
