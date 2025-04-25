import os
import re
import matplotlib.pyplot as plt
import numpy as np

# 可调整变量
font_size = 16
axis_font_size = 18
title_font_size = 20
names = ['autonuma', 'nomad', 'tpp','vtism']
colors = ['#d6a36f', '#E64B35', '#8dc2e9', '#3333cb']

# 解析日志，返回 MB 单位
def parse_log_file(file_path):
    node0_pages = []

    with open(file_path, 'r') as file:
        for line in file:
            match_node0 = re.search(r'NUMA Node 0: \d+ pages \((.+) MB', line)
            if match_node0:
                node0_pages.append(float(match_node0.group(1)))

    return node0_pages  # 返回纯数据，不用 index，统一在绘图时生成

# 对齐所有 trace，统一补到最长长度
def align_data(data_list):
    max_len = max(len(d) for d in data_list)
    aligned = []
    for d in data_list:
        if len(d) < max_len:
            padded = d + [d[-1]] * (max_len - len(d))
        else:
            padded = d
        aligned.append(padded)
    return aligned, list(range(max_len))

# 绘图
def plot_combined_data(log_files):
    raw_data = [parse_log_file(log) for log in log_files]
    aligned_data, iterations = align_data(raw_data)

    plt.figure(figsize=(10, 6))

    markers = ['o', 's', '^', 'D']  # 添加不同的 marker 样式

    for i, node0_data in enumerate(aligned_data):
        node0_data_gb = [mb / 1024 for mb in node0_data]
        plt.plot(
            iterations,
            node0_data_gb,
            color=colors[i],
            label=names[i],
            marker=markers[i],  # 使用不同 marker
            markersize=6,
            markerfacecolor=colors[i],
            markevery=3,
            linewidth=2
        )

    plt.xlabel('Time(second)', fontsize=axis_font_size)
    plt.ylabel("NUMA Node 0 Memory (GB)", fontsize=axis_font_size)
    plt.xticks(fontsize=font_size)
    plt.yticks(fontsize=font_size)
    plt.legend(fontsize=font_size, loc='upper left')
    plt.tight_layout()
    plt.savefig('promote.png', dpi=300)
    plt.savefig('promote.pdf', dpi=300)
    plt.close()

# 主程序
import sys
def main():
    r = 0
    log_files = [f'{name}.{r}.log' for name in names]
    plot_combined_data(log_files)

if __name__ == "__main__":
    main()
