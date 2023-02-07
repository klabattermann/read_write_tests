#!/bin/bash
#
# start a reader_single process, called by run_test_all.sh
#

set -u

reader_cmd=$(dirname $(abspath $0))/bin/rww_reader_single

rnd=$(( RANDOM * 10 * 1000 ))
offset=$(( 1024 * 1024 * 1024  * 10 + 3635 + rnd))

fn=${1:?}

[[ -e /tmp/wk_rww_1 ]] && rm -f /tmp/wk_rww_1
${reader_cmd} ${fn} ${offset}
