### Test Coverage Report Container ###
# Define builder stage
FROM crazy-code:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build targets/binaries
RUN cmake ..
RUN make
WORKDIR /usr/src/project/build_coverage

# Build and generate coverage report
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage
RUN cp -r /usr/src/project/build_coverage/report /usr/src/project/static_files/coverage_info