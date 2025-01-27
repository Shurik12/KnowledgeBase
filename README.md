[<img alt="KnowledgeBase — open-source data management application" width="400px" src="https://www.dqglobal.com/wp-content/uploads/2017/07/Data-Management.jpg" />](https://google.com/)

KnowledgeBase is an open-source data management application, that allow manage all of your data from other different services. 

## How To Get Yandex Music Token
The best way:
1. https://yandex-music.readthedocs.io/en/main/token.html

Or another way:
1. Singin to your account in yandex passport
2. Go to link https://oauth.yandex.ru/authorize?response_type=token&client_id=cc01d5dbc7de4925bdf07f33bd3dfd61
3. Copy access_token part from url

## Build
```bash
git clone https://github.com/Shurik12/KnowledgeBase.git && \
cd KnowledgeBase

# Create default profile
conan profile detect --force

# Create another (ex. debug) version of profile: cp and edit file
cp conan_profile /root/.conan2/profiles/debug

# Install debug version to build/ folder
conan install . --output-folder=build --build=missing --profile=debug && \
cd build && \
cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang-18 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-18 && \
cmake --build . && \
```

## Run

### Dev container
```bash
./restart.sh
```

### Command line
1. Put two files config.xml playlist_map.json to the binary (server) directory
```bash
# 1. Set config.xml file using example
cp config.xml playlist_map.json build/
cd build
./server
# in another (new) terminal window run client
./client
```
### Web
```bash
# 1. Build frontend using frontend/README.md
# 2. Set config.xml file using example
cp config.xml build/
cd build
./server
# 3. Open http://0.0.0.0:8080/
```

