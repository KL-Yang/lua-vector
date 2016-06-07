#!/bin/bash

./v3 bench1.lua &
pid=$!
pidstat -r -p $pid 1
