if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
export CFLAGS="-mmacosx-version-min=10.7";
export CXXFLAGS="-mmacosx-version-min=10.7";
export LDFLAGS="-mmacosx-version-min=10.7";
fi

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
# Link gcc-6 and g++-6 to their standard commands
sudo ln -s /usr/bin/gcc-6 /usr/local/bin/gcc;
sudo ln -s /usr/bin/g++-6 /usr/local/bin/g++;
# Export CC and CXX to tell cmake which compiler to use
export CC=/usr/bin/gcc-6;
export CXX=/usr/bin/g++-6;
# Check versions of gcc, g++ and cmake
gcc -v && g++ -v && cmake --version;
fi

wget https://github.com/cleishm/libcypher-parser/releases/download/v0.6.0/libcypher-parser-0.6.0.tar.gz
tar zxvpf libcypher-parser-0.6.0.tar.gz
rm libcypher-parser-0.6.0.tar.gz
cd libcypher-parser-0.6.0
if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
./configure --prefix=/usr/local CFLAGS='-fPIC -mmacosx-version-min=10.7' LDFLAGS='-mmacosx-version-min=10.7'
fi
if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
./configure --prefix=/usr/local CFLAGS='-fPIC'
fi
make clean check
sudo make install
cd ..
rm -rf libcypher-parser-0.6.0
