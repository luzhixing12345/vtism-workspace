import re
import matplotlib.pyplot as plt
import numpy as np

# 日志文件路径
log_file = "latency.log"

# 解析日志文件
def parse_numa_log(file_path):
    memory_sizes = []
    latencies = []
    bandwidths = []

    with open(file_path, "r") as f:
        for line in f:
            match = re.match(r"(\d+)\s+([\d.]+)\s+([\d.]+)", line.strip())
            if match:
                memory_sizes.append(int(match.group(1)))
                latencies.append(float(match.group(2)))
                bandwidths.append(float(match.group(3)))

    return np.array(memory_sizes), np.array(latencies), np.array(bandwidths)

# 解析数据
memory_sizes, latencies, bandwidths = parse_numa_log(log_file)

# 字体大小变量
axis_font_size = 18
tick_font_size = 16
legend_font_size = 15
line_width = 2.5
marker_size = 6

# 创建图形
fig, ax1 = plt.subplots(figsize=(10, 6))

# 绘制 Latency 曲线
ax1.set_xlabel("Workload Memory Size (GB)", fontsize=axis_font_size)
ax1.set_ylabel("Latency (ns)", fontsize=axis_font_size)
ax1.plot(memory_sizes, latencies, marker="o", linestyle="-", linewidth=line_width, markersize=marker_size, label="Latency", color='blue')
ax1.tick_params(axis="y", labelsize=tick_font_size)
ax1.tick_params(axis="x", labelsize=tick_font_size)
# 给 latencies[0] 的位置标记 y 轴数值
# 添加横向虚线，延伸到第一个数据点
# 在Y轴上标记数值
first_latency = latencies[0]
ax1.axhline(y=first_latency, xmin=0, xmax=0.05, linestyle=":", 
           color='grey', alpha=0.5, linewidth=1.5)
# ax1.axhline(y=latencies[0], xmin=0, xmax=0.1, linestyle="dashed")
yticks = ax1.get_yticks()
new_yticks = list(yticks)[1:-1] + [latencies[0]]
ax1.set_yticks(new_yticks)
# 添加图例
# fig.legend(loc="upper right", fontsize=legend_font_size, bbox_to_anchor=(0.95, 0.9))
ax1.axhline(y=288.4, xmin=0, xmax=1, linestyle="dashed", color='red')
ax1.text(15, 300, "288.4 ns (Local CXL DRAM Latency)", fontsize=legend_font_size, color='red')

ax1.axhline(y=486.4, xmin=0, xmax=1, linestyle="dashed", color='red')
ax1.text(15, 500, "486.4 ns (Remote CXL DRAM Latency)", fontsize=legend_font_size, color='red')
plt.grid(True, linestyle="--", alpha=0.6)

# 调整布局
plt.tight_layout()

# 保存图片
plt.savefig("dram_latency.png", dpi=300)

# 显示图形
# plt.show()
