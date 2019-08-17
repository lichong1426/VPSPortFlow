#!/bin/sh
cd /opt/netflow
nohup ./neflowStatistics >/dev/null 2>stderr.log &
