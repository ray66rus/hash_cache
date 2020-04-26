#include <iostream>

#include <hash_snapshot/hash_snapshot.hpp>

#include "hashcache.h"

using namespace hashcache;

int main()
{
    snapshot sshot(".hash_cache.txt");

    std::cout << HC_UTILITY_NAME << " utility, version "
        << HC_VER_MAJOR << "." << HC_VER_MINOR << "." << HC_VER_PATCH
        << std::endl << std::endl;

    std::cout << "Scanning current directory..." << std::endl;

    sshot.scan_working_dir();
    auto files_to_fix = sshot.modified_files_list();

    std::cout << "Scan finished. Number of files with changed modification time: " << files_to_fix.size() << std::endl;

    if( files_to_fix.size() == 0 ) {
        std::cout << "Nothing to fix. Exiting" << std::endl;
        sshot.save(); // Remember newly added file
        exit( 0 );
    }

    std::cout << std::endl << "Fixing files:" << std::endl;
    for( auto file_name: files_to_fix ) {
        std::cout << "Modification time of file \"" << file_name << "\" has changed. Fixing" << std::endl;

        sshot.fix_file( file_name );
    }

    sshot.save();
}
