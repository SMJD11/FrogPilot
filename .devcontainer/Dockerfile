FROM ghcr.io/commaai/openpilot-base:latest

RUN apt update && apt install -y vim net-tools usbutils htop ripgrep tmux wget mesa-utils xvfb libxtst6 libxv1 libglu1-mesa libegl1 gdb bash-completion



RUN usermod -aG video batman

USER batman

RUN cd $HOME && \
    curl -O https://raw.githubusercontent.com/commaai/agnos-builder/master/userspace/home/.tmux.conf && \
    curl -O https://raw.githubusercontent.com/commaai/agnos-builder/master/userspace/home/.vimrc
