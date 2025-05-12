FROM debian:bookworm

COPY --from=gcc:4 /usr/local/bin/* /usr/local/bin/
COPY --from=gcc:4 /usr/local/lib/* /usr/local/lib/
COPY --from=gcc:4 /usr/local/lib64/* /usr/local/lib64/
COPY --from=gcc:4 /usr/local/libexec/* /usr/local/libexec/
COPY --from=gcc:4 /usr/local/include/* /usr/local/include/
