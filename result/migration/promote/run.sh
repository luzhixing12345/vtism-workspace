#!/bin/bash

kernel_name=$(uname -r)

./numa_promote 0 2 > $kernel_name.log