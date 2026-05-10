FROM ubuntu:26.04@sha256:f3d28607ddd78734bb7f71f117f3c6706c666b8b76cbff7c9ff6e5718d46ff64 AS build

RUN apt update && \
    DEBIAN_FRONTEND=noninteractive \
    apt install --yes \
        autoconf \
        autoconf-archive \
        automake \
        build-essential \
        clang \
        curl \
        git \
        libtool \
        linux-libc-dev \
        ninja-build \
        pkg-config \
        zip \
        unzip \
        tar


ARG CMAKE_VERSION=4.3.2
ARG CMAKE_INSTALL_DIR=/opt/cmake-$CMAKE_VERSION
ARG USERNAME=ubuntu

RUN curl --location --output=/tmp/cmake.sh https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh
RUN mkdir --parents "$CMAKE_INSTALL_DIR" \
    && sh /tmp/cmake.sh --skip-license --prefix="$CMAKE_INSTALL_DIR" \
    && ln --symbolic --force "$CMAKE_INSTALL_DIR/bin/"* /usr/local/bin \
    && cmake --version


ARG VCPKG_ROOT="/home/$USERNAME/vcpkg"
ARG PATH="$PATH:$VCPKG_ROOT:/usr/local/bin"

USER $USERNAME

RUN git clone https://github.com/microsoft/vcpkg.git $VCPKG_ROOT \
    && $VCPKG_ROOT/bootstrap-vcpkg.sh -disableMetrics \
    && vcpkg --version

COPY ./ /app

WORKDIR /app

RUN cmake --workflow ci

ARG APP_HOME=/app

ENV LD_LIBRARY_PATH=$APP_HOME/lib:$LD_LIBRARY_PATH
ENV PATH=$APP_HOME/bin:$PATH
