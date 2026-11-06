FROM --platform=linux/amd64 ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install --yes --no-install-recommends sudo curl gnupg ca-certificates git python3-pip \
 && rm -rf /var/lib/apt/lists/*
RUN python3 -m pip install --break-system-packages "cmake==3.31.6" "meson==1.10.1"
COPY ci/setup-ubuntu-24.04.sh ci/ubuntu-24.04-packages.txt /opt/ci/
RUN bash /opt/ci/setup-ubuntu-24.04.sh /opt/spirv-llvm-translator 3>/opt/build.env && cat /opt/build.env \
 && rm -rf /var/lib/apt/lists/* /root/.cache /root/.ccache
RUN apt-get update && apt-get install --yes --no-install-recommends libdrm-dev libssl-dev autoconf && rm -rf /var/lib/apt/lists/*
RUN git config --system --add safe.directory '*'
COPY docker/entry.sh /opt/entry.sh
ENTRYPOINT ["/opt/entry.sh"]
