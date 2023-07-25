FROM ubuntu:jammy

ENV LANG=C.UTF-8 LANGUAGE=C LC_ALL=C.UTF-8
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && \
    apt-get upgrade -qq && \
    apt-get install -qq --no-install-recommends \
        sudo python3-pip wget cmake pkg-config gperf xz-utils patch gdb \
        build-essential libegl1-mesa-dev libxkbcommon-dev libatk-bridge2.0-dev \
        libatk1.0-dev libcairo2-dev libepoxy-dev libgbm-dev libgcrypt20-dev \
        libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev libharfbuzz-dev \
        libicu-dev libjpeg-dev liblcms2-dev libopenjp2-7-dev libsoup2.4-dev \
        libsqlite3-dev libsystemd-dev libtasn1-6-dev libwayland-dev \
        libwebp-dev libwoff-dev libxslt1-dev wayland-protocols libglib2.0-dev \
        libgtk-3-dev libfontconfig1-dev libfreetype6-dev libhyphen-dev \
        libmanette-0.2-dev libnotify-dev libxml2-dev libgudev-1.0-dev bison \
        flex libpng-dev libxt-dev libenchant-2-dev libsecret-1-dev libgl-dev \
        libgles-dev libjavascriptcoregtk-4.1-0 gstreamer1.0-plugins-base \
        gstreamer1.0-plugins-good gstreamer1.0-gl libgl1-mesa-dri patchelf \
        unifdef ruby-dev libgstreamer-plugins-bad1.0-dev && \
    pip3 install meson ninja

COPY webkit /root/
WORKDIR /root
RUN ./webkit --build

ENTRYPOINT [ "./webkit" ]
