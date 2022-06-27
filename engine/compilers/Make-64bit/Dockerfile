FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get -y install \
        build-essential \
        gcc-multilib \
        g++-multilib \
        nasm \
        libsdl-dev \
        libxft-dev \
        libopenal-dev && \
    rm -rf /var/lib/{apt,dpkg,cache,log}/
RUN mkdir /torque2d-engine-build/
