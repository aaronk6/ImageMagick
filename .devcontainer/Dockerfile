FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y autoconf gcc g++ git libtool locales make pkg-config gdb && \
    locale-gen en_US.UTF-8

ENV CFLAGS='-Wall -Wextra -Wno-builtin-declaration-mismatch'
ENV CXXFLAGS='-Wall -Wextra -Wno-builtin-declaration-mismatch'
