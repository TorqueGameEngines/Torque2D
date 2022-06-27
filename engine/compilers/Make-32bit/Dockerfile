FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture i386  &&\
    apt-get update && \
    apt-get -y install \
        build-essential \
        gcc-multilib \
        g++-multilib \
        nasm \
        libsdl-dev:i386 \
        libxft-dev:i386 \
        libopenal-dev:i386 && \
    rm -rf /var/lib/{apt,dpkg,cache,log}/
RUN mkdir /torque2d-engine-build/
