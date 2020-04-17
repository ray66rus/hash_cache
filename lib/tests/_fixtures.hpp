#ifndef CPP_HASHCACHE_SNAPSHOT_TEST_HPP_
#define CPP_HASHCACHE_SNAPSHOT_TEST_HPP_

#include <filesystem>

#include <catch2/catch.hpp>

#include <hash_snapshot/hash_snapshot.hpp>

#define FILE_EXCEPTION_MATCHER      Catch::Matchers::Predicate<hash_snapshot_exception>( \
                [] ( hash_snapshot_exception const& e ) -> \
                bool { return e.type() == hash_snapshot_exception::error_type::IO_ERROR; } \
            )

#define PARSER_EXCEPTION_MATCHER    Catch::Matchers::Predicate<hash_snapshot_exception>( \
                [] ( hash_snapshot_exception const& e ) -> \
                bool { return e.type() == hash_snapshot_exception::error_type::PARSE_ERROR; } \
            )

#define NORMAL_SNAPSHOT_SIZE    1010
#define FIXED_SNAPSHOT_SIZE     938

extern const std::filesystem::path& current_test_dir();

extern void create_test_directory();
extern void delete_test_directory();
extern void populate_test_directory( fs::path working_dir = "" );
extern const fs::path& get_test_file_path( int num );
extern void create_test_file( fs::path const& file_path );

extern const fs::path create_test_file_1();
extern const fs::path create_test_file_2();
extern const fs::path create_test_file_3();

extern const fs::path create_entry_1_file();

extern const std::string test_file_1_digest();

extern const fs::path create_empty_file();
extern const std::string empty_file_digest();

extern const hashcache::snapshot_entry& get_entry_1();
extern const std::string get_string_for_entry_1();
extern const fs::path get_file_path_for_entry_1();

extern const hashcache::snapshot_entry& get_entry_2();
extern const std::string get_string_for_entry_2();
extern const fs::path get_file_path_for_entry_2();

extern const fs::path minimal_snapshot_file_path();
extern void create_minmal_snapshot();
extern void create_problem_snapshot();

extern void change_file_ts( const std::string &file_name );
extern void change_file_contents( const fs::path& file_name, bool ensure_ts_change = false );
extern void revert_test_file_ts_to_default( const fs::path &file_name );

extern const fs::path normal_snapshot_file_name();
extern void create_normal_snapshot( fs::path const& file_name );

#endif
