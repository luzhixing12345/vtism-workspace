import json
import matplotlib.pyplot as plt
import numpy as np

# ======== 字体和线宽参数提取 ========
axis_font_size = 22
tick_font_size = 20
legend_font_size = 16
marker_font_size = 10
linewidth = 3

# ======== 加载 JSON 数据 ========
with open("data.json", "r") as f:
    data = json.load(f)

host_anon_pages_list = data["host_anon_pages_list"][:-600]
host_file_pages_list = data["host_file_pages_list"][:-600]
guest_anon_pages_list = data["guest_anon_pages_list"][:-600]
guest_file_pages_list = data["guest_file_pages_list"][:-600]

# 转换为 GB
for i in range(len(host_anon_pages_list)):
    host_anon_pages_list[i] = host_anon_pages_list[i] / 1024 / 1024
    host_file_pages_list[i] = host_file_pages_list[i] / 1024 / 1024
    guest_anon_pages_list[i] = guest_anon_pages_list[i] / 1024 / 1024
    guest_file_pages_list[i] = guest_file_pages_list[i] / 1024 / 1024

x = list(range(len(host_anon_pages_list)))

# 创建一个函数来选择均匀分布的标记点
def get_marker_positions(data, num_points=10):
    return np.linspace(0, len(data)-1, num_points, dtype=int)

# 创建图形
plt.figure(figsize=(10, 6))

# 绘图（使用统一样式）
plt.plot(x, host_anon_pages_list, label="Host VM Anon", color="#ff0000", linewidth=linewidth,
         marker='^', markersize=marker_font_size, markevery=len(x)//10)

plt.plot(x, host_file_pages_list, label="Host VM File", color="#ff0000", linewidth=linewidth,
         marker='o', markersize=marker_font_size, markevery=len(x)//10)

plt.plot(x, guest_anon_pages_list, label="Guest VM Anon", color="#0000ff", linewidth=linewidth,
         marker='^', markersize=marker_font_size, markevery=len(x)//10)

plt.plot(x, guest_file_pages_list, label="Guest VM File", color="#0000ff", linewidth=linewidth,
         marker='o', markersize=marker_font_size, markevery=len(x)//10)

# 标签、图例、刻度等字体大小
plt.xlabel("Time (minutes)", fontsize=axis_font_size)
plt.ylabel("Page Cache (GB)", fontsize=axis_font_size)
plt.xticks([])  # 隐藏 x 轴刻度值
plt.tick_params(axis="both", direction="in", length=6, labelsize=tick_font_size)
plt.legend(fontsize=legend_font_size)

# 显示网格
plt.grid(True, linestyle="-", alpha=0.7)

# 布局 & 保存
plt.tight_layout()
plt.savefig("anon_pages_comparison.png", dpi=300)
plt.savefig("anon_pages_comparison.pdf", dpi=300)
