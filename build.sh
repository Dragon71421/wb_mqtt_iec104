
# clean previous build files
rm -rf ./build

# create bin directory and open it
mkdir ./build
cd ./build

# build all
cmake ../
make