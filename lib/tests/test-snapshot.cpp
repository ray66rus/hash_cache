#include <_fixtures.hpp>
#include <iostream>
#include <sstream>

using namespace hashcache;

TEST_CASE( "Working directory doesn't exist", "[snapshot]" ) {

    create_test_directory();

    REQUIRE_THROWS_AS( snapshot( "_NONEXISTENT_", "not_important" ), std::ios_base::failure );

    delete_test_directory();
}

SCENARIO( "working directory or snapshot removed during workflow", "[snapshot]" ) {

    create_test_directory();

    fs::path working_dir = current_test_dir() / "will_be_killed";
    fs::create_directory( working_dir );

    REQUIRE( fs::is_directory( working_dir ) );

    snapshot sh( normal_snapshot_file_name().string(), working_dir.string() );

    WHEN( "working directory removed before rescan" ) {

        fs::remove_all( working_dir );

        REQUIRE_FALSE( fs::exists( working_dir ) );

        THEN( "rescan throws exception" ) {
            REQUIRE_THROWS_AS( sh.scan_working_dir(), fs::filesystem_error );
        }
    }

    WHEN( "snapshot file removed before rescan" ) {
        fs::remove( working_dir / normal_snapshot_file_name() );

        REQUIRE_FALSE( fs::exists( working_dir / normal_snapshot_file_name() ) );

        THEN( "throws exception" ) {
            REQUIRE_THROWS_AS( sh.scan_working_dir(), fs::filesystem_error );
        }
    }

    WHEN( "working directory removed before save" ) {
        populate_test_directory( working_dir );

        sh.scan_working_dir();

        fs::remove_all( working_dir );
        REQUIRE_FALSE( fs::exists( working_dir ) );

        THEN( "throws exception" ) {
            REQUIRE_THROWS( sh.save() );
        }
    }

    delete_test_directory();
}

SCENARIO( "logical workflow errors", "[snapshot]" ) {

    create_test_directory();

    snapshot sh( normal_snapshot_file_name().string() );

    GIVEN( "no scan done" ) {
        WHEN( "trying to get modified files list" ) {
            THEN( "prints warning and returns empty list" ) {

                std::stringstream buffer;
                std::streambuf* saved_buf = std::cerr.rdbuf( buffer.rdbuf() );

                auto list = sh.modified_files_list();

                std::cerr.rdbuf( saved_buf );

                REQUIRE( list.empty() );
                REQUIRE_THAT( buffer.str(), Catch::Contains( "directory wasn't scanned") );
            }
        }
    }

    GIVEN( "scan done" ) {

        sh.scan_working_dir();

        WHEN( "trying to revert ts of not existing file" ) {
            THEN( "silently do nothing" ) {
                REQUIRE_NOTHROW( sh.revert_file_ts( "_NONEXISTENT_" ) );
            }
        }
    }

    delete_test_directory();
}

SCENARIO( "normal workflow - first run", "[snapshot]" ) {

    fs::path initial_dir = fs::current_path();

    create_test_directory();
    populate_test_directory();

    fs::current_path( current_test_dir() );

    GIVEN( "working directory is not empty but doesn't have snapshot file" ) {

        snapshot sh( normal_snapshot_file_name().string() );

        REQUIRE( fs::exists( normal_snapshot_file_name() ) );

        WHEN( "scan working directory" ) {

            REQUIRE_NOTHROW( sh.scan_working_dir() );

            THEN( "no modified files" ) {

                REQUIRE( sh.modified_files_list().empty() );

                AND_WHEN( "snapshot saved" ) {

                    sh.save();

                    THEN( "snapshot file has the right size ") {
                        REQUIRE( fs::file_size( normal_snapshot_file_name() ) == normal_snapshot_size() );
                    }
                }
            }

        }
    }

    fs::current_path( initial_dir );
    delete_test_directory();
}

SCENARIO( "normal workflow", "[snapshot]" ) {

    fs::path initial_dir = fs::current_path();

    create_test_directory();
    populate_test_directory();

    fs::current_path( current_test_dir() );

    fs::path working_dir = "";

    GIVEN( "working directory is not empty and has valid snapshot file" ) {

        create_normal_snapshot( normal_snapshot_file_name() );
        snapshot sh( normal_snapshot_file_name().string() );

        AND_GIVEN( "some changes done ") {

            // remove two files
            fs::remove( working_dir / get_test_file_path( 0 ) );
            fs::remove( working_dir / get_test_file_path( 3 ) );

            // modify one file
            fs::path modified_file = get_test_file_path( 9 );
            change_file_contents( working_dir / modified_file, false );

            // revert timestamp for one file
            revert_test_file_ts_to_default( get_test_file_path( 7 ) );

            // add new file
            create_test_file( "new_test_file.test" );

            WHEN( "scan working directory" ) {

                REQUIRE_NOTHROW( sh.scan_working_dir() );
                auto modified_list = sh.modified_files_list();

                THEN( "modified file list length corresponds to changes made" ) {

                    REQUIRE( modified_list.size()  == 9 );

                    AND_THEN( "digest changed for only one file" ) {
                        for( auto const& fn: modified_list ) {
                            if( fn == modified_file ) {
                                REQUIRE( sh.is_file_contents_changed( fn ) );
                            } else {
                                REQUIRE_FALSE( sh.is_file_contents_changed( fn ) );
                            }
                        }

                        AND_WHEN( "fixes are made" ) {
                            for( auto const& fn: modified_list ) {
                                if( fn == modified_file ) {
                                    sh.update_file_digest( fn );
                                } else {
                                    sh.revert_file_ts( fn );
                                }
                            }

                            THEN( "modified file list is empty" ) {
                                REQUIRE( sh.modified_files_list().size() == 0 );

                                AND_WHEN( "snapshot saved" ) {
                                    sh.save();

                                    THEN( "snapshot file has the right size ") {
                                        REQUIRE( fs::file_size( normal_snapshot_file_name() ) == fixed_snapshot_size() );

                                        AND_WHEN( "file loaded and directory rescanned" ) {
                                            snapshot sh( normal_snapshot_file_name().string() );
                                            sh.scan_working_dir();

                                            THEN( "modified file list is empty" ) {
                                                REQUIRE( sh.modified_files_list().size() == 0 );
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        AND_WHEN( "fixes are made with shortcut function" ) {
                            for( auto const& fn: modified_list ) {
                                sh.fix_file( fn );
                            }

                            AND_WHEN( "snapshot saved" ) {
                                sh.save();

                                THEN( "snapshot file has the right size ") {
                                    REQUIRE( fs::file_size( normal_snapshot_file_name() ) == fixed_snapshot_size() );
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fs::current_path( initial_dir );
    delete_test_directory();

}
