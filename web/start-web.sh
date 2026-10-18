#!/usr/bin/env sh
# LIFT web: локальный сервер (браузер не грузит .wasm/.data через file://)
cd "$(dirname "$0")" || exit 1

echo
echo "  LIFT web: http://localhost:8080"
echo "  Остановить: Ctrl+C"
echo

python3 -m http.server 8080
