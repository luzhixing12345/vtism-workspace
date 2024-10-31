#!/bin/bash

pid=$1

rm -rf damon.*
rm -rf *.png

sudo damo record -o damon.data $pid
sudo damo report heatmap --output heatmap.png
sudo damo report wss --range 0 101 1 --plot wss.png
sudo damo report wss --range 0 101 1 --sortby time --plot wss_time.png
sudo damo report wss --range 0 101 1 --sortby size --plot wss_size.png
sudo damo report footprints --plot footprints.png
sudo damo report footprints --sortby time --plot footprints_time.png
sudo damo report footprints --sortby size --plot footprints_size.png

echo "generate heatmap.png wss.png wss_time.png wss_size.png footprints.png footprints_time.png footprints_size.png"