import re
import matplotlib.pyplot as plt
from collections import defaultdict

# 字体和样式参数
title_fontsize = 16
axis_fontsize = 16
tick_font_size = 14
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

    plt.xlabel("Workload Memory Size (GB)", fontsize=axis_fontsize)
    plt.ylabel("Average Scan Time (ms)", fontsize=axis_fontsize)
    # plt.title("Average Scan Time vs Memory Size", fontsize=title_fontsize)
    plt.xticks(memory_sizes, fontsize=tick_font_size)
    extra_yticks = []
    # for x in [1, 2,3]:
    #     if x in avg_times:
    #         y = avg_times[x]
    #         extra_yticks.append(y)
            # # 标注虚线
            # ax.axhline(y=y, linestyle='--', color=dashed_line_color, linewidth=1)
            # ax.text(memory_sizes[0] - 0.5, y + 0.3, f"{y:.1f} ms", color=dashed_line_color, fontsize=10)

    # 更新 y 轴刻度
    # current_yticks = list(ax.get_yticks())
    # new_yticks = sorted(set(current_yticks[1:-1] + extra_yticks))
    # ax.set_yticks(new_yticks)

    plt.yticks(fontsize=tick_font_size)
    plt.grid(True, linestyle='--', alpha=0.5)
    # plt.legend(fontsize=16)
    plt.tight_layout()
    plt.savefig("scan_time.png", dpi=300)
    plt.show()

if __name__ == "__main__":
    log_file = "scan_time.log"
    avg_times = parse_scan_time_log(log_file)
    plot_avg_scan_time(avg_times)
