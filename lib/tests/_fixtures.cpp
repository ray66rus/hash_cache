#include <_fixtures.hpp>

#include <iomanip>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#define EOL_SEQUENCE "\r\n"
#else
#define EOL_SEQUENCE "\n"
#endif

namespace fs = std::filesystem;
namespace chrono = std::chrono;
namespace hc = hashcache;

static fs::path TEST_DIR = std::string( "_testdir_" );
static const std::vector<fs::path> test_files {
    "test1.test", "test2.test", "Тест.test",
    "dir1/test11.test", "dir1/test12.test", "dir1/test13.test",
    "dir1/dir11/test111.test", "dir1/dir11/test112.test",
    "dir1/dir11/dir111/test1.test", "dir1/dir11/название каталога/test2.test",
    "dir2/test21.test", "dir2/test22.test",
    ".invisible1.test",
    ".invisible_dir/invisible.test", ".invisible_dir/.invisible_dir2/invisible2.test",
    "dir1/.invisible.test", "dir1/dir11/.invisible_dir/invisible.test"
};

static const std::vector<std::string> fixed_snapshot_vector {
    "2020-04-21 00:11:18.705167 075417d4b62b52ed67dc1e8e5c3b8405 dir1/dir11/dir111/test1.test",
    "2020-04-21 00:11:18.705167 fbfea27afc8f617f86b15b86d6dab0a6 dir1/dir11/test111.test",
    "2020-04-21 00:11:18.705167 fbfea27afc8f617f86b15b86d6dab0a6 dir1/dir11/test112.test",
    "2020-04-23 15:37:37.250942 6713843ae4e0c3ff4bb6aa8ce56ebc01 dir1/dir11/название каталога/test2.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir1/test12.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir1/test13.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir2/test21.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir2/test22.test",
    "2020-04-23 15:37:37.250942 4430ef7d49c5f2193402e587b34a8358 new_test_file.test",
    "2020-04-21 00:11:18.705167 075417d4b62b52ed67dc1e8e5c3b8405 test2.test",
    "2020-04-21 00:11:18.705167 2fed5e1b3c52b7264ac2fc773de21862 Тест.test",
};

static const std::vector<std::string> normal_snapshot_vector {
    "2020-04-21 00:11:18.705167 075417d4b62b52ed67dc1e8e5c3b8405 dir1/dir11/dir111/test1.test",
    "2020-04-21 00:11:18.705167 fbfea27afc8f617f86b15b86d6dab0a6 dir1/dir11/test111.test",
    "2020-04-21 00:11:18.705167 fbfea27afc8f617f86b15b86d6dab0a6 dir1/dir11/test112.test",
    "2020-04-21 00:11:18.705167 075417d4b62b52ed67dc1e8e5c3b8405 dir1/dir11/название каталога/test2.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir1/test11.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir1/test12.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir1/test13.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir2/test21.test",
    "2020-04-21 00:11:18.705167 f048f8e361e791722833784c1ad6a130 dir2/test22.test",
    "2020-04-21 00:11:18.705167 075417d4b62b52ed67dc1e8e5c3b8405 test1.test",
    "2020-04-21 00:11:18.705167 075417d4b62b52ed67dc1e8e5c3b8405 test2.test",
    "2020-04-21 00:11:18.705167 2fed5e1b3c52b7264ac2fc773de21862 Тест.test",
};

int64_t snapshot_ts_to_int64( std::string const& datetime, int64_t msec )
{
    std::stringstream ss( datetime );
    std::tm tm {};

    tm.tm_isdst = -1;
    ss >> std::get_time( &tm, "%Y-%m-%d %H:%M:%S" );

    auto filetime = chrono::system_clock::from_time_t( std::mktime( &tm ) );
    chrono::microseconds ms_since_epoch = chrono::duration_cast<chrono::microseconds>( filetime.time_since_epoch() );

    return ms_since_epoch.count() + msec;
}

int64_t snapshot_test_file_ts() { return snapshot_ts_to_int64( "2020-04-21 00:11:18", 705167 ); }

const fs::path& current_test_dir() { return TEST_DIR; }

void create_test_directory()
{
    if( fs::exists( current_test_dir() ) ) {
        delete_test_directory();
    }
    fs::create_directory( TEST_DIR );
}

void delete_test_directory()
{
    fs::remove_all( TEST_DIR );
}

const fs::path create_test_file_1()
{
    fs::path file_path = current_test_dir() / "test1.txt";
    std::ofstream ofs( file_path.string() );

    ofs << std::setfill( '1' ) << std::setw( 150000 ) << "";
    ofs.close();

    return file_path;
}

const fs::path create_test_file_2()
{
    fs::path file_path = current_test_dir() / "test2.txt";

    std::ofstream ofs( file_path.string() );
    ofs << std::setfill( '2' ) << std::setw( 100 ) << "";
    ofs.close();

    return file_path;
}

const fs::path create_test_file_3()
{
    fs::path file_path = current_test_dir() / "test3.txt";

    std::ofstream ofs( file_path.string() );
    ofs << std::setfill( '3' ) << std::setw( 100 ) << "";
    ofs.close();

    return file_path;
}

const std::string test_file_1_digest() { return "4f605b2ae8f9046dee8ebe960c08a774"; }

const fs::path create_empty_file()
{
    fs::path file_path = current_test_dir() / "empty.txt";

    std::ofstream ofs( file_path.string() );
    ofs.close();

    return file_path;
}

const std::string empty_file_digest() { return "d41d8cd98f00b204e9800998ecf8427e"; }

const fs::path get_file_path_for_entry_1() { return current_test_dir() / fs::path( "file1.dat" ); }
extern const hc::snapshot_entry get_entry_1()
{
    int64_t ts = snapshot_ts_to_int64( "2020-02-14 16:31:27", 236012 );
    return hc::snapshot_entry( "6d616d6e9a09ddb55b590ed59cc865c9", ts, 0 );
}
extern const std::string get_string_for_entry_1()
{
        return "2020-02-14 16:31:27.236012 6d616d6e9a09ddb55b590ed59cc865c9 " + get_file_path_for_entry_1().string();
}

const fs::path get_file_path_for_entry_2() { return current_test_dir() / fs::path( "dir1/file1_1.txt" ); }
extern const hc::snapshot_entry get_entry_2()
{
    int64_t ts = snapshot_ts_to_int64( "2020-02-26 21:14:35", 437918 );
    return hc::snapshot_entry( "b02e620a8f879bfcebb92357a6075c26", ts, 0 );
}
extern const std::string get_string_for_entry_2()
{
    return "2020-02-26 21:14:35.437918 b02e620a8f879bfcebb92357a6075c26 " + get_file_path_for_entry_2().string();
}

const fs::path minimal_snapshot_file_path() { return current_test_dir() / fs::path( ".hash_cache.txt" ); }

void create_minmal_snapshot()
{
    std::ofstream ofs( minimal_snapshot_file_path().string() );
    ofs << get_string_for_entry_1() << std::endl;
    ofs << get_string_for_entry_2() << std::endl;
    ofs.close();
}

void create_problem_snapshot()
{
    std::ofstream ofs( minimal_snapshot_file_path().string() );
    ofs << "some random garbage" << std::endl;
    ofs << get_string_for_entry_2() << std::endl;
    ofs.close();
}

const fs::path create_entry_1_file()
{
    fs::path file_path = get_file_path_for_entry_1();
    std::ofstream ofs( file_path.string() );

    ofs << std::setfill( '0' ) << std::setw( 10 ) << "";
    ofs.close();

    return file_path;
}

void change_file_ts( const std::string& file_name )
{
    std::this_thread::sleep_for( chrono::milliseconds( 10 ) );
    chrono::microseconds ms_since_epoch = chrono::duration_cast<chrono::microseconds>( chrono::system_clock::now().time_since_epoch() );
    hc::file_info::last_write_time( file_name, ms_since_epoch.count() );
}

void revert_test_file_ts_to_default( const fs::path &file_name )
{
    hc::file_info::last_write_time( file_name, snapshot_test_file_ts() );
}

void change_file_contents( const fs::path& file_name, bool ensure_ts_change )
{
    if( ensure_ts_change ) {
        std::this_thread::sleep_for( chrono::milliseconds( 10 ) );
    }

    std::ofstream ofs( file_name.string(), std::ofstream::app );
    ofs << "1";
    ofs.close();
}

void populate_test_directory( fs::path working_dir )
{
    if( working_dir == "" ) {
        working_dir = current_test_dir();
    }

    for( auto const& p: test_files ) {
        fs::path fname = p.filename();

        fs::path dirname = working_dir / p;
        dirname.remove_filename();

        if ( dirname != "" ) {
            fs::create_directories( dirname );
        }

        create_test_file( dirname / fname );
    }
}

void create_test_file( fs::path const& file_path )
{
    std::string fname = file_path.filename().string();
    std::ofstream ofs( file_path.string() );
    ofs << std::setfill( fname[ 0 ] ) << std::setw( fname.length() * 10000 ) << "";
    ofs.close();
}

const fs::path normal_snapshot_file_name() { return ".tmp_snapshot_file"; }

void create_normal_snapshot( fs::path const& file_name )
{
    std::ofstream ofs( file_name.string() );
    for( auto& line: normal_snapshot_vector ) {
        ofs << fs::path(line).lexically_normal().string() << std::endl;
    }
    ofs.close();
}

const fs::path& get_test_file_path( int num ) { return test_files[ num ]; }

int _snapshot_size( std::vector<std::string> const& vec )
{
    std::stringstream ss;

    std::for_each( vec.begin(), vec.end(), [ &ss ]( auto& s ) {
        ss << s << EOL_SEQUENCE;
    } );

    return ss.str().length();
}

int normal_snapshot_size() { return _snapshot_size( normal_snapshot_vector ); }

int fixed_snapshot_size() { return _snapshot_size( fixed_snapshot_vector ); }
