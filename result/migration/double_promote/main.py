import os
import re
import matplotlib.pyplot as plt
import numpy as np

# 可调整变量
font_size = 16
axis_font_size = 18
names = ['vtism', 'nomad']
colors = ['#3333cb', '#8dc2e9']  # vtism 蓝色，nomad 淡蓝

# 解析日志，返回 MB 单位
def parse_log_file(file_path, target_node):
    pages = []
    pattern = re.compile(rf'NUMA Node {target_node}:\s+\d+ pages \(([\d.]+) MB\)')

    with open(file_path, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                pages.append(float(match.group(1)))

    return pages

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
def plot_combined_data():
    all_data = []
    labels = []

    for i, name in enumerate(names):
        # 加载 _0 和 _1 的文件
        file_0 = f'{name}_0.log'
        file_1 = f'{name}_1.log'
        data_0 = parse_log_file(file_0, target_node=0)
        if name == 'nomad':
            data_1 = parse_log_file(file_1, target_node=0)
        else:
            data_1 = parse_log_file(file_1, target_node=1)
        all_data.append(data_0)
        all_data.append(data_1)
        labels.append(f'{name}-node0')
        labels.append(f'{name}-node1')

    aligned_data, iterations = align_data(all_data)

    plt.figure(figsize=(10, 6))

    for i, data in enumerate(aligned_data):
        data_gb = [mb / 1024 for mb in data]
        group_idx = i // 2  # 0: vtism, 1: nomad
        marker = 's' if i % 2 == 0 else 'o'
        plt.plot(
            iterations,
            data_gb,
            color=colors[group_idx],
            label=labels[i],
            marker=marker,
            markersize=6,
            markerfacecolor=colors[group_idx],
            markeredgecolor='black',
            markevery=3,
            linewidth=2
        )

    plt.xlabel('Time (second)', fontsize=axis_font_size)
    plt.ylabel('NUMA Node Memory (GB)', fontsize=axis_font_size)
    plt.xticks(fontsize=font_size)
    plt.yticks(fontsize=font_size)
    plt.legend(fontsize=font_size, loc='upper left')
    plt.tight_layout()
    plt.savefig('promote_compare.png', dpi=300)
    plt.close()

# 主程序
if __name__ == "__main__":
    plot_combined_data()
