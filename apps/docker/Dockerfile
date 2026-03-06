FROM ubuntu:24.04

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
    JOBS="$(nproc)"; \
    if [ "$JOBS" -gt 4 ]; then JOBS="$((JOBS / 4))"; fi; \
    if [ "$JOBS" -lt 1 ]; then JOBS=1; fi; \
    cmake --build . --parallel "$JOBS" && \
    cmake --install .

WORKDIR /opt/trinitycore/install
CMD ["bash"]
