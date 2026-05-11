FROM ubuntu:26.04@sha256:f3d28607ddd78734bb7f71f117f3c6706c666b8b76cbff7c9ff6e5718d46ff64

ARG APP_HOME=/app
ARG USERNAME=ubuntu

ENV LD_LIBRARY_PATH=$APP_HOME/lib
ENV PATH=$APP_HOME/bin:$PATH
ENV APP_HOME=$APP_HOME

ADD nel-tools-usd-*-Linux.tar.gz /tmp

RUN find /tmp && fail

RUN mv /tmp/nel-tools-usd-*-Linux /app

USER $USERNAME
WORKDIR $APP_HOME

ENTRYPOINT ["/app/bin/nel-tools-usd-usd-to-mesh"]
