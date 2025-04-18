import re
import matplotlib.pyplot as plt
from collections import defaultdict

# 字体和样式参数
axis_fontsize = 20
tick_font_size = 20
line_width = 2
marker_size = 6
line_color = "blue"
dashed_line_color = "#888888"
# 读取并解析日志文件
def parse_scan_time_log(filename):
    time_data = defaultdict(list)

    with open(filename, 'r') as f:
        for line in f:
            match = re.search(r'\((\d+) ms\).*?\[(\d+) GB\]', line)
            if match:
                time_ms = int(match.group(1))
                memory_gb = int(match.group(2))
                time_data[memory_gb].append(time_ms)

    return {k: sum(v)/len(v) for k, v in time_data.items()}

# 绘制折线图
def plot_avg_scan_time(avg_times):
    memory_sizes = sorted(avg_times.keys())
    avg_latencies = [avg_times[size] for size in memory_sizes]

    plt.figure(figsize=(10, 6))
    ax = plt.gca()
    ax.plot(memory_sizes, avg_latencies, marker='o', linestyle='-', color=line_color,
             linewidth=line_width, markersize=marker_size, label="Scan Time")

    plt.xlabel("Active Workload Memory Size (GB)", fontsize=axis_fontsize)
    plt.ylabel("Average Scan Time (ms)", fontsize=axis_fontsize)

    # 只保留偶数横坐标
    even_memory_sizes = [size for size in memory_sizes if size % 2 == 0]
    plt.xticks(even_memory_sizes, fontsize=tick_font_size)
    
    plt.yticks(fontsize=tick_font_size)
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.tick_params(axis="both", direction="in", length=6)
    plt.savefig("scan_time.png", dpi=300)
    # plt.show()


if __name__ == "__main__":
    log_file = "scan_time.log"
    avg_times = parse_scan_time_log(log_file)
    plot_avg_scan_time(avg_times)
