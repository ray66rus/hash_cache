## Interface

hash_snapshot::snapshot - Main class

snapshot(const std::string& file_name) - Constructor

void snapshot.load(void) - Load snapshot from file
void snapshot.rescan_dir(void) - Rescan directory, update file info including deleted and created files check
std::vector<std::string> snapshot.ts_updated_files_list() - Get files with updated ts
bool snapshot.is_file_changed(const std::string& file_name) - Check if file hash changed
void snapshot.fix_file_ts(const std::string& file_name) - Restore file timestamp
void snapshot.update_file_hash(const std::string &file_name) - Update file hash in DB
void snapshot.save(void) - Save snapshot fo file
