#include <_fixtures.hpp>

using namespace hashcache;

TEST_CASE( "File info init not found processing", "[fileinfo]" ) {
    REQUIRE_THROWS_MATCHES( file_info( "_NONEXISTENT_" ),
                hash_snapshot_exception,
                FILE_EXCEPTION_MATCHER
    );
}

TEST_CASE( "File info digest calculation", "[fileinfo]" ) {

    create_test_directory();

    const fs::path test_file_name = create_test_file_1();
    file_info test1_info( test_file_name );

    SECTION( "Digest validating" ) {

        const fs::path empty_file_name = create_empty_file();
        file_info empty_info( empty_file_name );

        REQUIRE( test1_info.get_digest() == test_file_1_digest() );
        REQUIRE( empty_info.get_digest() == empty_file_digest() );

    }

    SECTION( "File update changes digest" ) {

        change_file_contents( test_file_name );
        REQUIRE( file_info( test_file_name ).get_digest() != test_file_1_digest() );

    }

    delete_test_directory();
}

TEST_CASE( "File info timestamp check", "[fileinfo]" ) {

    create_test_directory();

    const fs::path test_file_name = create_test_file_1();
    int64_t ts = file_info::last_write_time( test_file_name );
    file_info test_info( test_file_name );

    SECTION( "Timestamp validating" ) {
        REQUIRE( test_info.get_ts() == ts );
    }

    SECTION( "File update changes timestamp" ) {

        change_file_contents( test_file_name, true );

        ts = file_info::last_write_time( test_file_name );
        REQUIRE( test_info.get_ts() < ts );

        REQUIRE( file_info( test_file_name ).get_ts() == ts );

    }

}
