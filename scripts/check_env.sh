
# check if python is python2

if [ "$(python -c 'import sys; print(sys.version_info[0])')" -ne "2" ]; then
    echo "Please install python2 first for ycsb redis benchmark and create symbolic link to python2"
    echo "sudo ln -s /usr/bin/python2 /usr/bin/python"
    exit 1
fi

# if exsist *.rdb file, remove it
if [ -f *.rdb ]; then
    rm *.rdb
    echo "Remove redis persistence data file *.rdb"
fi

# sudo vim /etc/apt/sources.list
#
# deb https://mirrors.ustc.edu.cn/ubuntu/ jammy main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ jammy main restricted universe multiverse
# deb https://mirrors.ustc.edu.cn/ubuntu/ jammy-updates main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ jammy-updates main restricted universe multiverse
# deb https://mirrors.ustc.edu.cn/ubuntu/ jammy-backports main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ jammy-backports main restricted universe multiverse
# deb https://mirrors.ustc.edu.cn/ubuntu/ jammy-security main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ jammy-security main restricted universe multiverse
# deb https://mirrors.ustc.edu.cn/ubuntu/ jammy-proposed main restricted universe multiverse
# deb-src https://mirrors.ustc.edu.cn/ubuntu/ jammy-proposed main restricted universe multiverse

# sudo apt install python2

sudo echo ""