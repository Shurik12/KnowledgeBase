FROM debian:latest

SHELL [ "/bin/bash", "-c" ]

WORKDIR /root

RUN \
  apt update -y && \
  apt install git wget vim g++ python3 cmake ninja-build lsb-release software-properties-common gnupg2 bash-completion -y && \
  echo "source /etc/profile.d/bash_completion.sh" >> ~/.bashrc && \
  echo "PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '" >> ~/.bashrc

RUN \
  wget -qO- https://apt.llvm.org/llvm.sh | bash -s -- 18 && \
  wget https://github.com/conan-io/conan/releases/download/2.10.1/conan-2.10.1-amd64.deb && \
  dpkg -i conan-2.10.1-amd64.deb && \
  rm -f conan-2.10.1-amd64.deb

COPY entrypoint.sh /

WORKDIR /root/Knowledgebase
ENTRYPOINT [ "/entrypoint.sh" ]