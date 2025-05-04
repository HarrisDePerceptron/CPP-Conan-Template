FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git curl wget python3-pip \
    python3-setuptools lcov ninja-build \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir --upgrade pip && \
    pip3 install --no-cache-dir "conan>=2.0.0,<3.0.0"

WORKDIR /app
COPY . .

RUN chmod +x scripts/build.sh scripts/run.sh scripts/run-test.sh
RUN ./scripts/build.sh --release
RUN ./scripts/run-test.sh

# -------------------------------
# Stage 2: Runtime
# -------------------------------
FROM ubuntu:22.04 AS runner

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd --create-home appuser
WORKDIR /home/appuser

# Copy dist folder with binary
COPY --from=builder /app/dist/ ./dist/

RUN chmod +x ./dist/Release/* 
USER appuser

WORKDIR /home/appuser/dist/Release

CMD ["sh", "-c", "./*"]
