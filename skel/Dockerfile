FROM jokeswar/base-ctl

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y python3-pip clang clang-tidy rsync valgrind

RUN pip install cpplint

COPY ./checker ${CHECKER_DATA_DIRECTORY}
