import os
import re
import matplotlib.pyplot as plt
import numpy as np

font_size = 12  # 可以随时修改字体大小
axis_font_size = 14 
names = ['autonuma','vtism', 'tpp', 'nomad']
colors = ['#d6a36f', '#8dc2e9', '#3333cb', '#E64B35']  # 可以随时修改颜色，这里用蓝色和绿色
# 提取 NUMA 节点的内存信息
def parse_log_file(file_path):
    # 初始化数据存储
    iterations = []
    node0_pages = []
    node2_pages = []
    
    with open(file_path, 'r') as file:
        iteration = 0
        for line in file:
            # match_iter = re.search(r'Random access iteration (\d+)/\d+ completed', line)
            match_node0 = re.search(r'NUMA Node 0: \d+ pages \((.+) MB', line)
            match_node2 = re.search(r'NUMA Node 2: \d+ pages \((.+) MB', line)
            
            # if match_iter:
            #     iteration = int(match_iter.group(1))
            #     iterations.append(iteration)
            if match_node0:
                node0_pages.append(float(match_node0.group(1)))
            elif match_node2:
                node2_pages.append(float(match_node2.group(1)))

    iterations = [i for i in range(len(node0_pages))]
    return iterations, node0_pages, node2_pages

# 绘制合并图
def plot_combined_data(log_files):
    plt.figure(figsize=(10, 6))

    # 颜色与图例句柄
    
    handles = [
        plt.Line2D([0], [0], color='black', lw=2, marker='s', markersize=6, markerfacecolor='white'),  # Node 0
        plt.Line2D([0], [0], color='black', lw=2, marker='o', markersize=6, markerfacecolor='white'),  # Node 2
    ] + [plt.Line2D([0], [0], color=colors[i], lw=2) for i in range(len(names))]  # 各内核颜色

    # 解析日志文件并绘图
    markerery = 3
    for i, log_file in enumerate(log_files):
        iterations, node0_data, node2_data = parse_log_file(log_file)

        # 设置 marker 间隔
        # marker_interval = 10
        # markers_node0 = ['s' if idx % marker_interval == 0 else None for idx in range(len(iterations))]
        # markers_node2 = ['o' if idx % marker_interval == 0 else None for idx in range(len(iterations))]

        # 画折线
        plt.plot(iterations, node0_data, color=colors[i], markerfacecolor=colors[i], markeredgecolor='black',
                 marker='s', markersize=6, markevery=markerery)
        plt.plot(iterations, node2_data, color=colors[i], markerfacecolor=colors[i], markeredgecolor='black',
                 marker='o', markersize=6, markevery=markerery)

    # 设置图例
    plt.legend(handles, ['Node 0', 'Node 2'] + names, loc='upper right', fontsize=font_size, bbox_to_anchor=(1, 0.9))

    # 保存图片
    plt.xlabel('Iteration', fontsize=axis_font_size)
    plt.ylabel("NUMA Node Memory (MB)", fontsize=axis_font_size)
    plt.tight_layout()
    plt.savefig(f'promote.png', dpi=300)
    plt.close()  # 关闭图形，避免内存泄漏

# 主程序
import sys
def main():
    r = sys.argv[1]
    log_files = []  # 你的日志文件列表
    for i in range(len(names)):
        log_files.append(f'{names[i]}.{r}.log')
        
    plot_combined_data(log_files)

if __name__ == "__main__":
    main()
