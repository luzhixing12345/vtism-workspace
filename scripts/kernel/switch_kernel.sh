#!/bin/bash

# Check if the script is run as root
if [ "$(id -u)" -ne "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

# Define the directory where the kernel images are stored
KERNEL_DIR="/boot"

# List available kernel versions and assign a number to each
echo "Available kernel versions:"
kernels=($(ls ${KERNEL_DIR}/vmlinuz-*))
count=0
for kernel in "${kernels[@]}"; do
    kernel_version=$(echo $kernel | sed 's/.*\/vmlinuz-//')
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
kernel_version=$(echo ${kernels[$kernel_number]} | sed 's/.*\/vmlinuz-//')
echo "Switching to kernel version: $kernel_version"

# Check if the selected kernel version exists
if [ ! -e "$KERNEL_DIR/vmlinuz-$kernel_version" ]; then
    echo "Kernel version $kernel_version does not exist"
    exit 1
fi

# Extract the menu entry for the default kernel
MID=`awk '/Advanced options.*/{print $(NF-1)}' /boot/grub/grub.cfg`
MID="${MID//\'/}"

KID=`awk -v kern="with Linux $kernel_version" '$0 ~ kern && !/recovery/ { print $(NF - 1) }' /boot/grub/grub.cfg`
KID="${KID//\'/}"

# Update GRUB configuration
sed -i "s/GRUB_DEFAULT=.*/GRUB_DEFAULT=\"$MID>$KID\"/" /etc/default/grub
update-grub

echo -e "\e[32;1mLinux kernel is now $kernel_version, Please reboot machine\e[0m"

echo -e "\e[1m  sudo reboot\e[0m"