import json
import matplotlib.pyplot as plt
import numpy as np

# 加载 JSON 数据
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

# 创建 X 轴 (数据点的索引)
x = list(range(len(host_anon_pages_list)))

# 创建一个函数来选择均匀分布的标记点
def get_marker_positions(data, num_points=10):
    # 计算均匀分布的位置
    return np.linspace(0, len(data)-1, num_points, dtype=int)

# 获取每条线的标记位置
host_anon_markers = get_marker_positions(host_anon_pages_list)
host_file_markers = get_marker_positions(host_file_pages_list)
guest_anon_markers = get_marker_positions(guest_anon_pages_list)
guest_file_markers = get_marker_positions(guest_file_pages_list)

# 创建图形
plt.figure(figsize=(10, 6))
linewidth = 2.5

# 绘制宿主机数据并在每条线上均匀添加标记
plt.plot(x, host_anon_pages_list, label="Host VM Anon", color="#ff1900", linewidth=linewidth, marker='o', markersize=5, markevery=len(x)//10)
plt.plot(x, host_file_pages_list, label="Host VM File", color="#ff8080", linewidth=linewidth, marker='s', markersize=5, markevery=len(x)//10)
plt.plot(x, guest_anon_pages_list, label="Guest Anon", color="#0000ff", linewidth=linewidth, marker='^', markersize=5, markevery=len(x)//10)
plt.plot(x, guest_file_pages_list, label="Guest File", color="#7f7fff", linewidth=linewidth, marker='D', markersize=5, markevery=len(x)//10)

# 添加标题和标签
plt.xlabel("Time (minutes)", fontsize=20)
plt.ylabel("Pages (GB)", fontsize=20)
plt.xticks([])  # 去掉 x 轴刻度
plt.tick_params(axis="both", direction="in", length=6)
plt.legend(fontsize=15)

# 设置字体大小
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)

# 显示网格
plt.grid(True, linestyle="-", alpha=0.7)

# 调整图像布局
plt.tight_layout()

# 保存图像到文件
plt.savefig("anon_pages_comparison.png", dpi=300)

# 显示图形
# plt.show()
