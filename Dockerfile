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

RUN chmod +x build.sh run.sh
RUN ./build.sh

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

RUN chmod +x ./dist/* 
USER appuser

CMD ["sh", "-c", "./dist/*"]
