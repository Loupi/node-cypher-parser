wget https://github.com/cleishm/libcypher-parser/releases/download/v0.6.0/libcypher-parser-0.6.0.tar.gz \
&& tar zxvpf libcypher-parser-0.6.0.tar.gz \
&& rm libcypher-parser-0.6.0.tar.gz \
&& cd libcypher-parser-0.6.0 \
&& ./configure --prefix=/usr/local CFLAGS='-fPIC' \
&& make clean check \
&& sudo make install \
&& cd .. \
&& rm -rf libcypher-parser-0.6.0