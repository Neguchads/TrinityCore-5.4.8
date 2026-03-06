FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ARG CMAKE_BUILD_TYPE=RelWithDebInfo

RUN apt-get update && apt-get install -y \
    ca-certificates \
    build-essential \
    gcc-12 \
    g++-12 \
    cmake \
    libssl-dev \
    libbz2-dev \
    libreadline-dev \
    libncurses-dev \
    libboost-all-dev \
    libmysqlclient-dev \
    libace-dev \
    mysql-client \
    tzdata \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/trinitycore
COPY . .

RUN mkdir -p build && cd build && \
    cmake .. \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
    -DCMAKE_INSTALL_PREFIX=/opt/trinitycore/install \
    -DCMAKE_C_COMPILER=gcc-12 \
    -DCMAKE_CXX_COMPILER=g++-12 && \
    cmake --build . --parallel "$(nproc)" && \
    cmake --install .

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    ca-certificates \
    mysql-client \
    tzdata \
    libssl3 \
    libbz2-1.0 \
    libreadline8t64 \
    libncurses6 \
    libmysqlclient21 \
    libace-7.1.2t64 \
    libboost-system1.83.0 \
    libboost-filesystem1.83.0 \
    libboost-program-options1.83.0 \
    libboost-thread1.83.0 \
    libboost-iostreams1.83.0 \
    libboost-regex1.83.0 \
    libboost-date-time1.83.0 \
    libboost-chrono1.83.0t64 \
    libboost-atomic1.83.0 \
    libboost-random1.83.0 \
    libboost-json1.83.0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/trinitycore
COPY --from=builder /opt/trinitycore/install /opt/trinitycore/install

WORKDIR /opt/trinitycore/install
CMD ["bash"]
