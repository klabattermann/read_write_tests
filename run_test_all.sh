#!/bin/bash
#
# Run the Write-while-reading test
# 

set -um

testfn="/cds/data/drpsrcf/temp/wktst/fs/f1"
prjdir=$(dirname $(abspath $0))

# Remove testfile so that read clients will wait until it exists
# the writer will fail if it exists
[[ -e ${testfn} ]] && rm -f ${testfn}

# start the reader, offset are set in run_reader.sh
pdsh -f 32 -w drp-srcf-cmp0[43-55] ${prjdir}/run_reader.sh ${testfn} &
sleep 2

# start writer on local host
#   rww_writer <test-file> <size[GB]> <sleep_between_writes[mus]>  
./bin/rww_writer ${testfn} 100 0 &

# wait for reader to finish, if done kill writer
wait %1
kill -s SIGINT %2
wait
