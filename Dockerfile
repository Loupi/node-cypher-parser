FROM mhart/alpine-node:10

WORKDIR /app

RUN apk add --no-cache make gcc g++ python

RUN wget https://github.com/cleishm/libcypher-parser/releases/download/v0.6.0/libcypher-parser-0.6.0.tar.gz \
&& tar zxvpf libcypher-parser-0.6.0.tar.gz \
&& rm libcypher-parser-0.6.0.tar.gz \
&& cd libcypher-parser-0.6.0 \
&& ./configure --prefix=/usr/local CFLAGS='-fPIC' \
&& make clean check \
&& make install \
&& cd .. \
&& rm -rf libcypher-parser-0.6.0

COPY ./package*.json ./binding.gyp /app/
COPY ./addon /app/addon

RUN npm install --unsafe-perm --production

COPY . .

CMD ["sleep", "600"]