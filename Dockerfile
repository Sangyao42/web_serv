FROM debian:bookworm

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        build-essential \
        make \
        g++ \
        valgrind \
        strace \
        git

COPY Makefile /web_serv/
COPY src /web_serv/src
WORKDIR /web_serv
RUN make
CMD tail -f /dev/null