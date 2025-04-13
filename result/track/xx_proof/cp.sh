#!/bin/bash

for interval in 1000 1500 2000 2500
do
    scp vm:~/${interval}* ${interval}/
done