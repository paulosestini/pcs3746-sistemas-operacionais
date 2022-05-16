FROM ubuntu:16.04
MAINTAINER Tiago Koji Castro Shibata

# Packages
RUN apt-get update && apt-get install --no-install-recommends -y bc binutils-arm-linux-gnueabi \
    build-essential cpio gcc-arm-linux-gnueabi gdb-arm-none-eabi libc6-dev-armel-cross \
    libelf-dev locales qemu-system-arm sudo && \
    rm -rf /var/lib/apt/lists/*

# Environment
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV MAKEFLAGS -j4

# Non-root "student" user
RUN useradd -mG sudo student && mkdir -p /etc/sudoers.d && \
    echo '%sudo   ALL=(ALL:ALL) NOPASSWD: ALL' > /etc/sudoers.d/sudo_nopasswd && \
    mkdir -p /home/student/src && \
    echo "alias gdb=\"arm-none-eabi-gdb -ex 'target remote:1234' /home/student/src/linux/vmlinux\"" >> /home/student/.bashrc

COPY ./entrypoint.sh /
COPY ./default_cmd.sh /
ENTRYPOINT ["/entrypoint.sh"]
CMD ["/default_cmd.sh"]
