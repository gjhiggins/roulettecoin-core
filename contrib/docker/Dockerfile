# 611 (vcore) docker iamge

FROM debian:jessie

MAINTAINER Graham Higgins <gjh@bel-epa.com> (thxto Florian Fuessl <flo@degnet.de>)
ENV REFRESHED_AT 20161008

RUN groupadd -r vcore && useradd -r -m -g vcore vcore

# grab the latest V Core source code from git, compile, install and clean up
RUN set -ex \
    && apt-get update \

    # install dependencies
    && apt-get install -y --no-install-recommends ca-certificates wget \
        build-essential libssl-dev libboost-all-dev git \

    # grab gosu for easy step-down from root
    && gpg --keyserver pool.sks-keyservers.net --recv-keys B42F6819007F00F88E364FD4036A9C25BF357DD4 \
    && wget -qO /usr/local/bin/gosu "https://github.com/tianon/gosu/releases/download/1.7/gosu-$(dpkg --print-architecture)" \
    && wget -qO /usr/local/bin/gosu.asc "https://github.com/tianon/gosu/releases/download/1.7/gosu-$(dpkg --print-architecture).asc" \
    && gpg --verify /usr/local/bin/gosu.asc \
    && rm /usr/local/bin/gosu.asc \
    && chmod +x /usr/local/bin/gosu \

    # compile and install V Core
    && cd /usr/src \
    && git clone https://github.com/gjhiggins/vcoincore.git vcore \
    && cd vcore/contrib \
    && ./build-static-with-db48.sh \
    && cp ../src/vcored /usr/local/bin \
    && strip /usr/local/bin/vcored \

    # cleanup
    && cd ../.. && rm -rf /usr/src/vcore \
    && apt-get -y purge build-essential libssl-dev libboost-all-dev git \
    && apt-get -y autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV vcore_DATA /vcore
RUN mkdir $vcore_DATA \
    && chown vcore:vcore $vcore_DATA \
    && ln -s $vcore_DATA /home/vcore/.vcore
VOLUME ["/vcore"]

COPY docker-entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]

EXPOSE 5530 5531
CMD ["vcored"]
