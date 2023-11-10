FROM gcc:9

WORKDIR /game
RUN apt-get update && apt-get install -y cmake

COPY . /game
RUN mkdir /game/build && cd /game/build && cmake ..
RUN cd /game/build && make && tests/test