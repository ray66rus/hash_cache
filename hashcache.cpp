#include <iostream>

#include <hash_snapshot/hash_snapshot.hpp>

using namespace hashcache;

int main()
{
    snapshot sshot(".hash_cache.txt");

    sshot.scan_working_dir();

    for( auto file_name: sshot.modified_files_list() ) {

        if( sshot.is_file_contents_changed( file_name ) ) {
            sshot.update_file_digest( file_name );
        } else {
            sshot.revert_file_ts( file_name );
        }
    }

    sshot.save();
}
