#!/usr/bin/env bash
set -euo pipefail

LOG_DIR="/var/log/fogpass"
target=${1:-/tmp/fogpass-logs.tar.gz}

tar -czf "$target" -C "$LOG_DIR" .
echo "Logs exported to $target"
