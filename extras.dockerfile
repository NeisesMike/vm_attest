ARG USER_BASE_IMG=trustworthysystems/sel4
FROM $USER_BASE_IMG

# This dockerfile is a shim between the images from Dockerhub and the 
# user.dockerfile.
# Add extra dependencies in here!

# For example, uncomment this to get cowsay on top of the sel4/camkes/l4v
# dependencies:

RUN apt-get update -q \
    && apt-get install -y sudo \
    && apt-get install -y meson \
    && apt-get clean autoclean \
    && apt-get autoremove --yes \
    && rm -rf /var/lib/{apt,dpkg,cache,log}/ \
    && curl https://cakeml.org/cake-x64-32.tar.gz > cake-x64-32.tar.gz \
    && tar -xvzf cake-x64-32.tar.gz \
    && cd cake-x64-32 \
    && make cake \
    && mv cake /usr/bin/cake32
