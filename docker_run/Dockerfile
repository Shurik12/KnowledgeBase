FROM debian:latest

SHELL [ "/bin/bash", "-c" ]

WORKDIR /root

RUN \
  apt update -y && \
  apt install wget vim software-properties-common gnupg2 bash-completion -y && \
  echo "source /etc/profile.d/bash_completion.sh" >> ~/.bashrc && \
  echo "PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '" >> ~/.bashrc && \
  mkdir Knowledgebase

WORKDIR /root/Knowledgebase

CMD [ "tail", "-f", "/dev/null" ]