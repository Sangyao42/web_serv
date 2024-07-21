FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y \
        build-essential \
        make \
        g++ \
        valgrind \
        strace \
		python3 \
        git

COPY Makefile /web_serv/
COPY src /web_serv/src
COPY www /web_serv/www
COPY config /web_serv/config
WORKDIR /web_serv
RUN make
CMD ./webserv ./config/default_docker.conf
