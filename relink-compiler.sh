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