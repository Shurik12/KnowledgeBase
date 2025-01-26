FROM debian:latest

SHELL [ "/bin/bash", "-c" ]
WORKDIR /root
COPY conan_profile /root/

RUN \
	apt update -y && \
	apt install git wget vim python3 cmake ninja-build lsb-release software-properties-common gnupg2 -y && \
	bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && \
	wget https://github.com/conan-io/conan/releases/download/2.10.1/conan-2.10.1-amd64.deb && \
	dpkg -i conan-2.10.1-amd64.deb && \
	rm -f conan-2.10.1-amd64.deb && \
	git clone https://github.com/Shurik12/KnowledgeBase.git && \
	cd KnowledgeBase && \
	mkdir /root/run && \
	cp playlists_map.json /root/run/ && \
	conan profile detect --force && \
	cp /root/conan_profile /root/.conan2/profiles/debug && \
	conan install . --output-folder=build --build=missing --profile=debug && \
	cd build && \
	cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang-18 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-18 && \
	cmake --build . && \
	cp server client /root/run/

