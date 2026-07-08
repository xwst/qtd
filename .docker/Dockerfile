FROM debian:forky
 
ARG UID=1000
ARG GID=1000

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y \
        qt6-base-dev-tools \
        qt6-base-dev \
        qt6-declarative-dev \
        qt6-tools-dev \
        qt6-tools-dev-tools \
        qt6-l10n-tools \
        qml6-module-qtquick \
        qml6-module-qtquick-controls \
        qml6-module-qtquick-layouts \
        qml6-module-qttest \
        cmake \
        build-essential \
        sqlite3 \
        ninja-build \
        pkg-config \
        git \
        curl \
        wget \
        ca-certificates \
        libgl1-mesa-dri \
        libegl1 \
        libxkbcommon-x11-0 \
        mesa-utils \
        locales \
        clang \
        clang-tidy \
        gdb \
        lldb \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd -g ${GID} appuser \
    && useradd -m -u ${UID} -g ${GID} -s /bin/bash appuser

RUN mkdir -p /home/appuser/.local/share/opencode \
             /home/appuser/.local/state/opencode \
             /home/appuser/.cache \
    && chown -R appuser:appuser /home/appuser/.local /home/appuser/.cache
WORKDIR /workspace

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen \
    && locale-gen


RUN mkdir -p /var/tmp/opencode && chmod 777 /var/tmp/opencode
USER appuser
RUN curl -fsSL https://opencode.ai/install | bash

ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

ENV QT_QPA_PLATFORM=offscreen
ENV QT_QUICK_BACKEND=software

ENV PATH="/home/appuser/.opencode/bin:${PATH}"
ENV TMPDIR=/var/tmp/opencode
