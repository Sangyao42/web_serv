FROM debian:bookworm

RUN apt-get update && \
    apt-get upgrade && \
    apt-get install -y \
        build-essential \
        make \
        g++ \
        valgrind \
        strace \
        git

COPY . /web_serv
WORKDIR /web_serv
CMD tail -f /dev/null