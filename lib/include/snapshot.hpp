#include <string>
#include <vector>

#include "hash_snapshot_export.h"

class snapshot {
    public:
        HASH_SNAPSHOT_EXPORT snapshot( const std::string &file_name );

        HASH_SNAPSHOT_EXPORT void load();
        HASH_SNAPSHOT_EXPORT void rescan_dir();
        HASH_SNAPSHOT_EXPORT std::vector<std::string> ts_updated_files_list();
        HASH_SNAPSHOT_EXPORT bool is_file_changed( const std::string &file_name );
        HASH_SNAPSHOT_EXPORT void fix_file_ts( const std::string &file_name );
        HASH_SNAPSHOT_EXPORT void update_file_hash( const std::string &file_name );
        HASH_SNAPSHOT_EXPORT void save( void );
    
    private:
        void dummy();
};