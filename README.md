# HASH CACHE

`hascahe` utility is a small program that creates in the given directory “snapshot” file, containing hashsums and modification times for all files in that directory and it’s subdirectories (excluding “hidden” files and directories, i.e. the ones that starts with “.”). When this utility runs, it compares current state of the files, found in this directory and its subdirectories, with the data in this snapshot file and finds files, which modification time has changed. If such a file was really changed (hashsum is different from the state, fixed in the snapshot file), its hashsum and modification time in the snapshot updated. Otherwise, its modification time forced to that from the snapshot.

This utility is based on the `hash_snapshot` library that is also in this repository.

## Building

This sofrware is written as a cross-platrofm application, so it should builds and works in any system that meets the following building requirements. Build was tested under Ubuntu 16 and Windwos 10 platrofms.

### Building requirements

For building `hash_snapshot` library and `hashcache` utility you should have:

*   CPP-compiler, supporting C\++17 revision of c++
*   CMake: version >= 3.8
*   OpenSSL library, version >= 1.0.1
*   Catch2 test framework
*   Ninja build system, version >= 1.3.0

### Ubuntu 16

Tested configuration

*   Ubuntu 16.04.6 LTS (x86_64)
*   gcc, version 8.4.0
*   cmake, version 3.17.0

#### GCC toolchain

To install gcc-8/g++\-8 on Ubuntu 16 run the following command:

    $ sudo apt update -y &&
    sudo apt upgrade -y &&
    sudo apt install build-essential software-properties-common -y &&
    sudo add-repository ppa:ubuntu-toolchain-r/test -y &&
    sudo apt update -y &&
    sudo apt install gcc-8 g++-8 -y &&
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8 &&
    sudo update-alternatives --config gcc

Check gcc version:

    $ gcc --version
    gcc (Ubuntu 8.4.0-1ubuntu1~16.04.1) 8.4.0
    Copyright (C) 2018 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


#### CMake

Since the default version of CMake in Ubuntu 16 is pretty old, you should upgrade to the latest version of CMake:

    $ sudo apt purge --auto-remove cmake
    $ wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
    $ sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ `lsb_release -cs` main"
    $ sudo apt update && sudo apt install cmake

Check CMake version:

    $ cmake --version
    cmake version 3.17.1
    CMake suite maintained and supported by Kitware (kitware.com/cmake).

#### Ninja build system

Install Ninja build system:

    $ sudo apt install ninja-build

Check Ninja version:

    $ ninja --version
    1.5.1

#### Alternative way to install toolchain

To install gcc/cmake/ninja you can run `install-toolchain.sh` script from the root of this repository

#### Build dependencies

`hash_snapshot` library depends on `OpenSSL` library and `Catch2` test framework.

To install OpenSSL development package, run

    $ sudo apt install -y libssl-dev

To install Catch2 test framework clone it from the github repository:

    $ git clone https://github.com/catchorg/Catch2.git

then build and install:

    $ cd Catch2
    $ cmake -Bbuild -H. -DBUILD_TESTING=OFF
    $ sudo cmake --build build/ --target install

#### Building the hashcache utility and hash_snapshot library

Clone the `hash_cache` repository

    $ git clone https://github.com/ray66rus/hash_cache.git

Build it

    $ cd hash_cache
    $ cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
    $ cmake --build build/

To check that the library works as intended, go to `build/lib` directory and run CTest:

    $ cd build/lib
    $ ctest

To install the `hash_snapshot` library system-wide, build/install it with `cmake` and then run `ldconfig`:

    $ sudo cmake --build build/ --target install && ldconfig

### Windows 10

Tested configuration

*   Windows 10 Enterprise Evaluation (64-bit): version 10.0.17763
*   Visual Studio 2019 Community 2019: version 16.5.4

#### Visual Studio

Be sure that the following components are installed:

*   Workload “Desktop development for C++”
*   Individual component “Git for Windows”

#### Ninja build system

Install it either with `Chocolatey` package manager (recommended) or any other method described at [ninja github page](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages). To install it with `Chocolatey`, run `Windows Power Shell` with `Administrator` privileges and in command line type:

    > choco install -y ninja

#### Build dependencies

`hash_snapshot` library depends on `OpenSSL` library and `Catch2` test framework. The easiest way to install them is to use [Vcpkg](https://github.com/microsoft/vcpkg) package manager.

To install `vcpkg`, run `Git CMD` command from Windows Start menu, and run the following commands in the console:
 
    > git clone https://github.com/Microsoft/vcpkg.git
    > cd vcpkg
    > bootstrap-vcpkg.bat
    > vcpkg integrate install

After `vcpkg` will be installed (it required `Administrator` privileges), install `openssl` and `catch2` packages:
    
    > vcpkg install openssl catch2 --triplet x64-windows

#### Building the hashcache utility and hash_snapshot library

1. Open Visual Studio
2. Select “Clone or check out code”
3. Enter “[https://github.com/ray66rus/hash_cache.git](https://github.com/ray66rus/hash_cache.git)” into the “Repository location” field and press Enter.
4. Wait until the repository is cloned and cmake cache generation is finished
5. Build project (“Build” -> “Build all” in application menu or just “F7”).

By default, “Debug” configuration is built. To build “Release”, after the project is cloned, do the following:

1. Click on the “Configuration” drop-down in the main toolbar and choose “Manage Configurations”
2. Click “+” button
3. Select “x64-Release” in the appeared menu
4. In the “Configuration type” field select “Release”
5. Save new configuration by pressing “Ctrl+s”

After that it should be possible to select “x64-Release” configuration from “Configuration” drop-down. Re-build cmake cache (“Project”->”CMake Cache (x64-Release Only)”->”Delete cache”, “Project”->”Generate Cache for HashCache”) and build the project.

## Using hashcache utility

### Run requirements
To run the utility you should have `openssl` and `hash_snapshot` libraries installed in your system.

#### Ubuntu 16
To install `openssl` library for ubuntu, run

    $ sudo apt install openssl

`hash_snapshot` library can be installed using as discribed in [Building the hashcache utility and hash_snapshot library](#building-the-hashcache-utility-and-hash_snapshot-library) section.

#### Windows 10
To install `openssl` library for Windows you can use any binary distribution for Windows x64 platform - for example, you can download and install it from [https://slproweb.com/products/Win32OpenSSL.html](https://slproweb.com/products/Win32OpenSSL.html).
`hash_snapshot.dll` file should be manually copied to the system dll directory (C:\Windows\System32) or be in the same directory with the executable file.

### Runing the utility
Copy executable file to the directory for which you want to make a snapshot and run it:

Linux:

    > $ ./hashcache
 
Windows:

    > hashcache.exe

First run can take a lot of time - depending on the amount of files and sub-directories in the working directory. After the first run a file named `.hash_cache.txt` will be created in the working directory. Don’t delete or manually edit this file - if you delete it, the snapshot information will be lost and the next run of the utility will need to re-create this file.


## Using of hash_shapshot library

All functions of the `hash_snapshot` library are provided via the single class `hashcache::snapshot`.

### Member functions

#### Constructor

##### hashcache::snapshot( std::string const& file_name )
Creates object using the current directory as the working directory.

Parameters:

* file_name: name of the snapshot file

##### hashcache::snapshot( std::string const& file_name, std::string const& dir, bool follow_symlinks = false )
Creates object with explicitly stated working directory.

Parameters:

* file_name: name of the snapshot file
* dir: working directory to scan (by default the current directory is used)
* follow_symlinks: whether to follow symlinks
    
#### Methods

##### void hashcache::scan_working_dir()
Loads snapshot file and scans working directory for files and sub-directories. All problems with parsing snapshot file and getting information about the filesystem objects are reported to the standard error stream.

New files, created after the last scan, are silently added to the snapshot.

If no snapshot is found, creates it.

##### std::vector<std::string> hashcache::modified_files_list()
Gets the list of file pathes which modification times are different from the saved in the snapshot file. 

New files, created after the last scan, are not appeared in this list. All pathes are relative to the 
working directory.

This function should be called after calling `scan_working_dir()`. If there was no call to `scan_working_dir()` prior the calling this function, empty list will be returned and the warning will be printed to the standard error stream.

##### bool hashcache::is_file_contents_changed( const std::string& file_name )

Checks if the file hashsum is different from the saved in the snapshot file.

Parameters:

* file_name: file path to check (relative to the working directory)

##### void hashcache::revert_file_mtime( const std::string& file_name )

Sets file modification time to the value from the snapshot file. Parameters:

* file_name: file path to set modification time (relative to the working directory)

##### void hashcache::update_file_digest( const std::string& file_name )

Calculates file hashsum and add it to the snapshot for this file. Also updates timestamp for this file

Parameters:

* file_name: file path to calculate digest (relative to the working directory)

##### void hashcache::save( void )
Saves the snapshot file. Should be called before the object destruction, otherwise all the changes made by `scan_working_dir` and `update_file_digest` functions won’t be saved

##### void hashcache::fix_file( const std::string& file_name )

“Shortcut” for the sequence of calls

    if( is_file_contents_changed( file_name, info ) ) {
    	update_file_digest( file_name, info );
    } else {
    	revert_file_mtime( file_name );
    }

Works more effective becase calculates file hashsum only once.

### Usage example

    hashcache::snapshot sshot(".hash_cache.txt");
    
    sshot.scan_working_dir();
    
    for( auto file_name: sshot.modified_files_list() ) {
        if( sshot.is_file_contents_changed( file_name ) ) {
             sshot.update_file_digest( file_name );
        } else {
             sshot.revert_file_mtime( file_name );
        }
    }

    sshot.save();

## TODO
*   Make the utility output more infromative
*   Add command line parameters (like "wokring directory", "follow symlink", "verbosity level"to the utility
*   Fix “follow_symlinks” option - check for cyclic links structure and test other edge cases

## License
`hashcache` utility and `hash_snapshot` library are available under [Mozilla Public License Version 2.0](https://www.mozilla.org/en-US/MPL/2.0/).

This software uses [OpenSSL library](https://www.openssl.org/), which is available under [Apache License Version 2.0](https://www.openssl.org/source/apache-license-2.0.txt)

This software uses [Catch2](https://github.com/catchorg/Catch2) test framework, which is available under [Boost Software License Version 1.0](https://www.boost.org/LICENSE_1_0.txt)


## Author

[Pavel Tolstov](mailto:ray66rus@gmail.com)
