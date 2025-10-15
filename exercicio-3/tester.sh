#!/usr/bin/env bash
set -euo pipefail

# ---- CONFIG ----
CODE1=${1:-}
CODE2=${2:-}
INPUT_GEN=${3:-}

# ---- VALIDATION ----
if [[ -z "$CODE1" || -z "$CODE2" || -z "$INPUT_GEN" ]]; then
    echo "Uso: ./difftest.sh <code1.c> <code2.cpp> <input.cpp>"
    exit 1
fi

if [[ ! -f "$CODE1" || ! -f "$CODE2" || ! -f "$INPUT_GEN" ]]; then
    echo "Erro: um dos arquivos não existe."
    exit 1
fi

# ---- BUILD ----
echo "[*] Compilando programas..."
gcc -O2 -o code1_bin "$CODE1" -lm -pthread
g++ -O2 -o code2_bin "$CODE2" -lm -pthread
g++ -O2 -o input_bin "$INPUT_GEN"

if [[ ! -x code1_bin || ! -x code2_bin || ! -x input_bin ]]; then
    echo "Erro: falha na compilação."
    exit 1
fi

# ---- LOOP ----
echo "[*] Iniciando differential testing infinito..."
ITER=0
while true; do
    ITER=$((ITER+1))
    # Gera entrada aleatória
    INPUT=$(./input_bin)

    # Executa os programas
    OUT1=$(./code1_bin $INPUT)
    OUT2=$(./code2_bin $INPUT)

    if [[ "$OUT1" != "$OUT2" ]]; then
        echo
        echo "Diferença detectada na iteração $ITER!"
        echo "Entrada: $INPUT"
        echo
        echo "Saída code1:"
        echo "$OUT1"
        echo
        echo "Saída code2:"
        echo "$OUT2"
        exit 1
    fi

    # Feedback visual
    echo "[$ITER] OK (entrada: $INPUT)"
done