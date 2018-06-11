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
