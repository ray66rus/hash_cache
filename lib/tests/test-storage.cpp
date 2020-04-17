#include <_fixtures.hpp>

using namespace hashcache;

TEST_CASE( "Storage basic init and load", "[storage]" ) {

    snapshot_storage ss;

    create_test_directory();

    SECTION( "Bad storage file checks" ) {

        REQUIRE_THROWS_AS( ss.init( current_test_dir() / "bad_dir/.qwerty" ), std::ios_base::failure );

        REQUIRE_NOTHROW( ss.init( minimal_snapshot_file_path() ) );

        std::filesystem::remove( minimal_snapshot_file_path() );
        REQUIRE_THROWS( ss.load() );

    }

    SECTION( "Init empty" ) {

        REQUIRE_NOTHROW( ss.init( minimal_snapshot_file_path() ) );
        REQUIRE( ss.size() == 0 );
        REQUIRE( ss.empty() );
        REQUIRE( std::filesystem::is_regular_file( minimal_snapshot_file_path()) );
        REQUIRE( std::filesystem::file_size(  minimal_snapshot_file_path() ) == 0 );

    }

    SECTION( "Init two entries snapshot" ) {

        create_minmal_snapshot();

        REQUIRE_NOTHROW( ss.init(  minimal_snapshot_file_path() ) );
        REQUIRE_NOTHROW( ss.load() );
        REQUIRE( ss.size() == 2 );
        REQUIRE_FALSE( ss.empty() );

    }

    SECTION( "Init two entries snapshot with one bad entry" ) {

        create_problem_snapshot();

        REQUIRE_NOTHROW( ss.init( minimal_snapshot_file_path() ) );
        REQUIRE_NOTHROW( ss.load() );
        REQUIRE( ss.size() == 1 );
        REQUIRE_FALSE( ss.empty() );

    }

    delete_test_directory();
}

SCENARIO( "Storage entries manipulation", "[storage]" ) {
    create_test_directory();
    create_minmal_snapshot();

    GIVEN( "storage is init and loaded") {
        snapshot_storage ss;
        ss.init( minimal_snapshot_file_path() );
        ss.load();

        WHEN( "processing unknown entry" ) {
            const fs::path foo_name = create_test_file_1();
            file_info foo_info( foo_name );

            THEN( "out of range error is thrown" ) {
                REQUIRE_THROWS_AS( ss.update_entry_metadata( "_NONEXISTENT_", foo_info ), std::out_of_range );
            }
        }

        AND_WHEN( "adding/removing entries" ) {
            const fs::path test_file_name = create_test_file_1();
            file_info test_info( test_file_name );

            THEN( "storage size changes accordingly" ) {
                REQUIRE_NOTHROW( ss.add_entry( test_file_name.string(), test_info ) );
                REQUIRE( ss.size() == 3 );

                REQUIRE_NOTHROW( ss.remove_entry( "_NONEXISTENT_" ) );
                REQUIRE( ss.size() == 3 );

                ss.remove_entry( test_file_name.string() );
                REQUIRE( ss.size() == 2 );

                ss.remove_entry( get_file_path_for_entry_1().string() );
                REQUIRE( ss.size() == 1 );
            }
        }

        AND_WHEN( "adding more entries" ) {
            const fs::path entry_3_file_name = create_test_file_1();
            const fs::path entry_4_file_name = create_test_file_2();

            file_info test_info_1( entry_3_file_name );
            file_info test_info_2( entry_4_file_name );

            ss.add_entry( entry_3_file_name.string(), test_info_1 );
            ss.add_entry( entry_4_file_name.string(), test_info_2 );

            THEN( "changed ts list is empty" ) {
                std::vector<std::string> const changed_files_1 = ss.get_ts_changed_files_list();
                REQUIRE( changed_files_1.empty() );

                AND_WHEN( "some files changed" ) {
                    const fs::path entry_1_file_name = create_entry_1_file(); // Make file for the entry 1 with the different timestamp and the digest
                    change_file_ts( entry_3_file_name.string() ); // Change timestamp for test file 1 (entry 3) but leave the digest untoched

                    file_info test_info_1_new( entry_1_file_name );
                    file_info test_info_3_new( entry_3_file_name );
                    file_info test_info_4_new( entry_4_file_name );

                    AND_WHEN( "metadata for all existing files updated" ) {
                        ss.update_entry_metadata( get_file_path_for_entry_1().string(), test_info_1_new );
                        ss.update_entry_metadata( entry_3_file_name.string(), test_info_3_new );
                        ss.update_entry_metadata( entry_4_file_name.string(), test_info_4_new );

                        std::vector<std::string> const changed_files_2 = ss.get_ts_changed_files_list();

                        THEN( "changed ts list is not empty" ) {
                            REQUIRE( changed_files_2.size() == 2 ); // entries 1 and 3
                        }

                        AND_THEN( "changes are consistent with what we've done to files" ) {
                            for( auto file_name: changed_files_2 ) {
                                if( file_name == entry_1_file_name ) {
                                    REQUIRE( ss.is_file_contens_changed( file_name, test_info_1_new.get_digest() ) );
                                } else if( file_name == entry_3_file_name ) {
                                    REQUIRE_FALSE( ss.is_file_contens_changed( file_name, test_info_3_new.get_digest() ) );
                                } else {
                                    REQUIRE( false );
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    delete_test_directory();
}

SCENARIO( "new snapshot entries adding and saving", "[storage]" ) {

    GIVEN( "Empty snapshot created" ) {

        create_test_directory();

        snapshot_storage ss;
        ss.init( minimal_snapshot_file_path() );
        ss.load();

        REQUIRE( ss.empty() );

        AND_GIVEN( "Three records added" ) {

            std::time_t rescan_time = _HASH_SNAPSHOT_TIMET_NOW;

            fs::path entry_1_file_name = create_test_file_1();
            file_info test_info_1( entry_1_file_name );
            ss.add_entry( entry_1_file_name.string(), test_info_1 );

            const fs::path entry_2_file_name = create_test_file_2();
            file_info test_info_2( entry_2_file_name );
            ss.add_entry( entry_2_file_name.string(), test_info_2 );

            REQUIRE( ss.size() == 2 );

            WHEN( "Snapshot saved" ) {

                ss.save( rescan_time );

                THEN( "After load should have two records" ) {

                    snapshot_storage ss_new_1;
                    ss_new_1.init( minimal_snapshot_file_path() );
                    ss_new_1.load();

                    REQUIRE( ss.size() == 2 );

                    AND_WHEN( "Saved without default last seen time" ) {

                        ss.save();

                        THEN( "After load should again have two records" ) {

                            snapshot_storage ss_new_2;
                            ss_new_2.init( minimal_snapshot_file_path() );
                            ss_new_2.load();

                            REQUIRE( ss_new_2.size() == 2 );

                        }

                    }

                    AND_WHEN( "Saved with too big rescan time") {

                        ss.save( rescan_time + 1 );

                        THEN( "Should be empty" ) {

                            snapshot_storage ss_new_2;
                            ss_new_2.init( minimal_snapshot_file_path() );
                            ss_new_2.load();

                            REQUIRE( ss_new_2.empty() );

                        }

                    }

                }

            }

            delete_test_directory();
        }
    }
}

SCENARIO( "existing snapshot storage entries updating and saving", "[storage]" ) {

    GIVEN( "two-entries snapshot loaded" ) {

        create_test_directory();

        create_minmal_snapshot();

        snapshot_storage ss;
        ss.init( minimal_snapshot_file_path() );
        ss.load();

        REQUIRE( ss.size() == 2 );

        AND_GIVEN( "Two records added" ) {

            std::time_t rescan_time = _HASH_SNAPSHOT_TIMET_NOW;

            const fs::path entry_1_file_name = create_test_file_1();
            file_info test_info_1( entry_1_file_name );
            ss.add_entry( entry_1_file_name.string(), test_info_1 );

            const fs::path entry_2_file_name = create_test_file_2();
            file_info test_info_2( entry_2_file_name );
            ss.add_entry( entry_2_file_name.string(), test_info_2 );

            REQUIRE( ss.size() == 4 );

            AND_GIVEN( "One record updated" ) {

                const fs::path entry_1_file_name = create_entry_1_file();
                file_info entry_1_info( entry_1_file_name );

                // Check that we really change the digest for this entry
                std::string new_digest = entry_1_info.get_digest();
                REQUIRE( ss.is_file_contens_changed( entry_1_file_name.string(), new_digest ) );

                ss.update_entry( entry_1_file_name.string(), entry_1_info );

                WHEN( "Snapshot saved" ) {

                    ss.save( rescan_time );

                    THEN( "After load should have three records" ) {

                        snapshot_storage ss_new;
                        ss_new.init( minimal_snapshot_file_path() );
                        ss_new.load();

                        REQUIRE( ss_new.size() == 3 );

                     }

                    AND_THEN( "Updated digest and timestamp should be saved" ) {

                        snapshot_storage ss_new;
                        ss_new.init( minimal_snapshot_file_path() );
                        ss_new.load();

                        REQUIRE_FALSE( ss_new.is_file_contens_changed( entry_1_file_name.string(), new_digest ) );

                    }

                }

            }

        }

        delete_test_directory();

    }

}

SCENARIO( "existing snapshot storage entries meta updating and saving", "[storage]" ) {

    GIVEN( "two-entries snapshot loaded" ) {

        create_test_directory();

        create_minmal_snapshot();

        snapshot_storage ss;
        ss.init( minimal_snapshot_file_path() );
        ss.load();

        REQUIRE( ss.size() == 2 );

        AND_GIVEN( "One record meta updated" ) {

            std::time_t rescan_time = _HASH_SNAPSHOT_TIMET_NOW;

            const fs::path entry_1_file_name = create_entry_1_file();
            file_info entry_1_info( entry_1_file_name );
            ss.update_entry_metadata( entry_1_file_name.string(), entry_1_info );

            WHEN( "Snapshot saved" ) {

                ss.save( rescan_time );

                THEN( "After load should be one record" ) {

                    snapshot_storage ss_new;
                    ss_new.init( minimal_snapshot_file_path() );
                    ss_new.load();

                    REQUIRE( ss_new.size() == 1 );

                }

            }

        }

        delete_test_directory();

    }

}

