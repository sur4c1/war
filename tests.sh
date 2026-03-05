#!/bin/bash

#!/usr/bin/env bash

TESTDIR="/tmp/test"

RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
RESET="\033[0m"

TOTAL=0
BEHAV_OK=0
BEHAV_KO=0
SIG_OK=0
SIG_KO=0

# ---- binaries to test (comment to disable) ----
TARGETS=(
hostname
which
ls
pwd
id
whoami
uname
date
)

# ---- custom command overrides ----
declare -A TESTS
TESTS[which]="which ls"
TESTS[ls]="ls /tmp"
TESTS[uname]="uname -a"
TESTS[date]="date +%s"

printf "\n=== Pestilence Functional Test ===\n\n"

run_test() {
    local bin="$1"
    local infected="$TESTDIR/$bin"

    if [[ ! -x "$infected" ]]; then
        printf "%-14s ${YELLOW}SKIP${RESET} (missing in /tmp/test)\n" "$bin"
        return
    fi

    local orig
    orig=$(command -v "$bin")

    if [[ ! -x "$orig" ]]; then
        printf "%-14s ${YELLOW}SKIP${RESET} (no original)\n" "$bin"
        return
    fi

    TOTAL=$((TOTAL+1))

    local cmd="${TESTS[$bin]}"

    if [[ -z "$cmd" ]]; then
        cmd="$bin"
    fi

    # run original
    out_orig=$(timeout 2 bash -c "$cmd" 2>&1)
    code_orig=$?

    # run infected
    out_test=$(timeout 2 bash -c "$TESTDIR/$cmd" 2>&1)
    code_test=$?

    printf "%-14s " "$bin"

    if [[ "$out_orig" == "$out_test" && "$code_orig" == "$code_test" ]]; then
        printf "behavior: ${GREEN}OK${RESET}   "
        BEHAV_OK=$((BEHAV_OK+1))
    else
        printf "behavior: ${RED}KO${RESET}   "
        BEHAV_KO=$((BEHAV_KO+1))
    fi

    if grep -a -q "Pestilence" "$infected"; then
        printf "signature: ${GREEN}OK${RESET}\n"
        SIG_OK=$((SIG_OK+1))
    else
        printf "signature: ${RED}KO${RESET}\n"
        SIG_KO=$((SIG_KO+1))
    fi
}

for bin in "${TARGETS[@]}"; do
    run_test "$bin"
done

printf "\n=== SUMMARY ===\n\n"

printf "Total tested:      %d\n" "$TOTAL"
printf "Behavior OK:       ${GREEN}%d${RESET}\n" "$BEHAV_OK"
printf "Behavior KO:       ${RED}%d${RESET}\n" "$BEHAV_KO"
printf "Signature OK:      ${GREEN}%d${RESET}\n" "$SIG_OK"
printf "Signature KO:      ${RED}%d${RESET}\n" "$SIG_KO"

printf "\n"
