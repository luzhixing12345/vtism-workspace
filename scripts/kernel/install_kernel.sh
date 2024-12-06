#!/bin/bash

DEB_PATH=/home/lzx/kernel-deb/

new_kernel_name=$(ls linux-headers-* | sed 's/linux-headers\-//')

if [ -z "$new_kernel_name" ]; then
    echo "No new kernel found"
    exit 1
fi
mv linux-headers-${new_kernel_name} $DEB_PATH
mv linux-image-${new_kernel_name} $DEB_PATH
rm linux-libc-* linux-upstream*

kernels=($(ls ${DEB_PATH}/linux-headers-*))
count=0
for kernel in "${kernels[@]}"; do
    kernel_version=$(echo $kernel | sed 's/.*\/linux-headers\-//')
    echo "[$count]: $kernel_version"
    ((count++))
done
echo ""

# Prompt the user to select a kernel version by number
read -p "Enter the number of the kernel version you want to switch to: " kernel_number

# Check if the input is a number and within the range
if ! [[ "$kernel_number" =~ ^[0-9]+$ ]] || [ "$kernel_number" -lt "0" ] || [ "$kernel_number" -ge "$count" ]; then
    echo "Invalid selection"
    exit 1
fi

# Get the kernel version based on the number
kernel_version=$(echo ${kernels[$kernel_number]} | sed 's/.*\/linux-headers\-//')
echo "Install kernel version: $kernel_version"

sudo dpkg -i ${DEB_PATH}/linux-headers-${kernel_version}.deb
sudo dpkg -i ${DEB_PATH}/linux-image-${kernel_version}.deb

echo "finished"
