#include <_fixtures.hpp>

using namespace hashcache;

TEST_CASE( "Snapshot entry_string/entry creation", "[entry]" ) {

    SECTION( "Snapshot entry init" ) {

        snapshot_entry empty;
        REQUIRE( empty.last_seen == 0 );
        REQUIRE_FALSE( empty.ts_changed );

    }

    SECTION( "Snapshot string entry format parsing" ) {

        REQUIRE_THROWS_MATCHES( shapshot_entry_string( "" ),
                    hash_snapshot_exception,
                    PARSER_EXCEPTION_MATCHER
        );

        REQUIRE_THROWS_MATCHES( shapshot_entry_string( "2020-02-14 16:31:27 digest" ),
                    hash_snapshot_exception,
                    PARSER_EXCEPTION_MATCHER
        );

        REQUIRE_THROWS_MATCHES( shapshot_entry_string( "2020-02-14 invalid digest filename" ),
                    hash_snapshot_exception,
                    PARSER_EXCEPTION_MATCHER
        );

        REQUIRE_THROWS_MATCHES( shapshot_entry_string( "invalid 16:31:27 digest filename" ),
                    hash_snapshot_exception,
                    PARSER_EXCEPTION_MATCHER
        );

        REQUIRE_NOTHROW( shapshot_entry_string( "2020-02-14 16:31:27 digest filename" ) );
        REQUIRE_NOTHROW( shapshot_entry_string( "2020-02-14 16:31:27 digest filename with spaces" ) );

    }

}

TEST_CASE( "Snapshot entry_string/entry conversion test", "[entry]" ) {

    SECTION( "Entry from string" ) {

        snapshot_entry entry1 = get_entry_1();
        shapshot_entry_string es1( get_string_for_entry_1() );
        snapshot_entry test_entry( es1 );

        REQUIRE( test_entry.digest == entry1.digest );
        REQUIRE( test_entry.ts == entry1.ts );
        REQUIRE( test_entry.last_seen == entry1.last_seen );
        REQUIRE_FALSE ( test_entry.ts_changed );

    }

    SECTION( "String from entry" ) {

        std::string es2_str = get_string_for_entry_2();
        shapshot_entry_string test_es( get_file_path_for_entry_2().string(), get_entry_2() );

        REQUIRE( std::string( test_es ) == es2_str );

    }

}
