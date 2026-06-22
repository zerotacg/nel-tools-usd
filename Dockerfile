FROM ubuntu:26.04@sha256:53958ec7b67c2c9355df922dd08dbf0360611f8c3cdb656875e81873db9ffdba AS build

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

RUN git clone https://github.com/microsoft/vcpkg.git $VCPKG_ROOT \
    && $VCPKG_ROOT/bootstrap-vcpkg.sh -disableMetrics \
    && vcpkg --version


WORKDIR /build
COPY --parents ./vcpkg.json ./vcpkg-configuration.json ./vcpkg-ports/ /build/
RUN vcpkg install

COPY ./ /build
RUN cmake --workflow docker


FROM ubuntu:26.04@sha256:53958ec7b67c2c9355df922dd08dbf0360611f8c3cdb656875e81873db9ffdba

ARG APP_HOME=/app
ARG USERNAME=ubuntu

ENV LD_LIBRARY_PATH=$APP_HOME/lib
ENV PATH=$APP_HOME/bin:$PATH
ENV APP_HOME=$APP_HOME

COPY --from=build /build/build/install/ci /app

USER $USERNAME

ENTRYPOINT ["/app/bin/nel-tools-usd-usd-to-mesh"]
