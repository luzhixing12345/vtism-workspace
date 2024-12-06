#!/bin/bash

newuser=$1
sudo adduser $1
sudo usermod -aG sudo $1
su $1
cd