#!/bin/bash
sudo dpkg -i linux-headers-*.deb
sudo dpkg -i linux-image-*.deb

echo "installed new kernel"

rm linux-headers-*.deb
rm linux-image-*.deb
rm linux-libc-*.deb
rm linux-upstream_*

echo "cleaned deb files"

echo "finished"
