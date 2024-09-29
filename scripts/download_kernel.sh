
kernel_version=$1
basic_url=https://mirrors.edge.kernel.org/pub/linux/kernel/v5.x/linux-${kernel_version}.tar.xz
download_position=~/code/
wget $basic_url -O $download_position/linux-${kernel_version}.tar.xz
cd $download_position
tar xf linux-${kernel_version}.tar.xz
echo "Kernel ${kernel_version} downloaded"