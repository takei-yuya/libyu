FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    build-essential \
 && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=gcc:5 /usr/local/bin/* /usr/local/bin/
COPY --from=gcc:5 /usr/local/lib/* /usr/local/lib/
COPY --from=gcc:5 /usr/local/lib64/* /usr/local/lib64/
COPY --from=gcc:5 /usr/local/libexec/* /usr/local/libexec/
COPY --from=gcc:5 /usr/local/include/* /usr/local/include/
