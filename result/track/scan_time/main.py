import re
import matplotlib.pyplot as plt
from collections import defaultdict

# 统一样式参数
axis_fontsize = 20
tick_font_size = 20
line_width = 2
marker_size = 6
line_color = "blue"
red_color = "red"

# 第一个图的解析函数（pr_scan_time）
def parse_pr_scan_log(log_file):
    page_values = []
    times = []

    log_pattern = re.compile(r"A/T: (\d+)/(\d+) \((\d+) ")

    with open(log_file, 'r') as file:
        for line in file:
            match = log_pattern.search(line)
            if match:
                page_value = int(match.group(2))
                time_value = int(match.group(3)) / 1000  # us -> ms
                page_values.append(page_value)
                times.append(time_value)

    return page_values, times

# 第二个图的解析函数（scan_time）
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

# 绘图
# 绘图
def plot_combined(pr_page_values, pr_times, avg_times):
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 6))

    # 子图 1：pr_scan_time
    ax1a = ax1
    ax1b = ax1a.twinx()

    l1, = ax1a.plot(range(len(pr_page_values)), pr_page_values, color=line_color, label="Scanned PTE items")
    l2, = ax1b.plot(range(len(pr_times)), pr_times, color=red_color, label="Scan Time")

    ax1a.set_xlabel('Time (minutes)', fontsize=axis_fontsize)
    ax1a.set_ylabel('Scanned PTE items', fontsize=axis_fontsize)
    ax1b.set_ylabel('Scan Time (ms)', fontsize=axis_fontsize)
    ax1a.tick_params(axis='both', labelsize=tick_font_size, direction="in", length=6)
    ax1b.tick_params(axis='y', labelsize=tick_font_size, direction="in", length=6)

    # 添加图例到第一张图中（左上角）
    ax1a.legend(handles=[l1, l2], loc='upper right', fontsize=axis_fontsize - 8)

    # 子图 2：scan_time
    memory_sizes = sorted(avg_times.keys())
    avg_latencies = [avg_times[size] for size in memory_sizes]

    ax2.plot(memory_sizes, avg_latencies, marker='o', linestyle='-', color=line_color,
             linewidth=line_width, markersize=marker_size)
    ax2.set_xlabel("Active Workload(GB)", fontsize=axis_fontsize)
    ax2.set_ylabel("Average Scan Time(ms)", fontsize=axis_fontsize)
    ax2.set_xticks([s for s in memory_sizes if s % 4 == 0])
    ax2.tick_params(axis='both', labelsize=tick_font_size, direction="in", length=6)
    ax2.grid(True, linestyle='--', alpha=0.5)

    plt.tight_layout()
    plt.savefig("combined_scan_time.png", dpi=300)
    plt.savefig("combined_scan_time.pdf", dpi=300)


if __name__ == "__main__":
    pr_log_file = "pt_scan_pr.log"
    scan_log_file = "scan_time.log"
    pr_page_values, pr_times = parse_pr_scan_log(pr_log_file)
    avg_times = parse_scan_time_log(scan_log_file)
    plot_combined(pr_page_values, pr_times, avg_times)
