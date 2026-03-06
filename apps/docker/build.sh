#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

IMAGE_NAME="trinitycore548"
IMAGE_TAR="${IMAGE_NAME}.tar"

if ! docker image inspect "${IMAGE_NAME}" >/dev/null 2>&1; then
  if [[ ! -f "${IMAGE_TAR}" ]]; then
    echo "Image ${IMAGE_NAME} was not found and ${IMAGE_TAR} is missing."
    echo "Run create-image.bat on a build machine and copy this folder, including ${IMAGE_TAR}."
    exit 1
  fi
  echo "Loading image ${IMAGE_NAME} from ${IMAGE_TAR}..."
  docker load -i "${IMAGE_TAR}"
fi

docker compose up -d --no-build
