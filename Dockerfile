FROM debian:bookworm

RUN apt-get update && \
    apt-get upgrade && \
    apt-get install -y \
        build-essential \
        cmake \
        make \
        g++ \
        valgrind \
        strace \
        git && \
    git clone https://github.com/google/googletest.git -b v1.14.0 && \
    cd googletest && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

COPY . /web_serv
WORKDIR /web_serv
CMD tail -f /dev/null