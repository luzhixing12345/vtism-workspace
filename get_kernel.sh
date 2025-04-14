#!/bin/bash

# 定义不同的内核路径
case "$1" in
    nomad)
        echo "/home/lzx/Nomad/src/nomad/arch/x86/boot/bzImage"
        ;;
    tpp)
        echo "/home/lzx/Nomad/src/linux-5.13-rc6/arch/x86/boot/bzImage"
        ;;
    vtism)
        echo "/home/lzx/vtism/arch/x86/boot/bzImage"
        ;;
    memtis)
        echo "/home/lzx/memtis/linux/arch/x86/boot/bzImage"
        ;;
    autonuma)
        echo "/home/lzx/code/linux-5.13.1/arch/x86/boot/bzImage"
        ;;
    *)
        echo "Error: Unknown kernel variant '$1'" >&2
        echo "available variants: nomad, tpp, vtism, memtis"
        exit 1
        ;;
esac
