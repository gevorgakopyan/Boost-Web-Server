### Build/test container ###
# Define builder stage
FROM crazy-code:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure --verbose

# Build and generage coverage report
WORKDIR /usr/src/project/build_coverage
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage
RUN cp -r /usr/src/project/build_coverage/report /usr/src/project/static_files/coverage_info

### Deploy container ###
# Define deploy stage
FROM ubuntu:focal as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/configs/main_config .
COPY --from=builder /usr/src/project/static_files static_files
COPY --from=builder /usr/src/project/credentials credentials

COPY --from=builder /usr/lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
#  Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["main_config"]
