# iDDS-Module
iDDS Modules implementation for openDAQ

## Build instructions
```
mkdir build
cd build
cmake ..
make -j
```

## Usage
Before running any executables, set up the required environment variables using the generated script:
```
source setenv.sh
```
This script updates the LD_LIBRARY_PATH with the necessary library paths.

Run Examples from build/bin folder. All examples expect the rtps.ini file to be in the same directory in order to run.
