import json
import matplotlib.pyplot as plt

# 加载 JSON 数据
with open("data.json", "r") as f:
    data = json.load(f)

host_anon_pages_list = data["host_anon_pages_list"][:-500]
guest_anon_pages_list = data["guest_anon_pages_list"][:-500]

for i in range(len(host_anon_pages_list)):
    host_anon_pages_list[i] = host_anon_pages_list[i] / 1024 / 1024
    guest_anon_pages_list[i] = guest_anon_pages_list[i] / 1024 / 1024

# 创建 X 轴 (数据点的索引)
x = list(range(len(host_anon_pages_list)))

# 绘制图形
plt.figure(figsize=(10, 6))

# 绘制宿主机数据
plt.plot(x, host_anon_pages_list, label="Host", color="#e94234", linewidth=4)

# 绘制虚拟机数据
plt.plot(x, guest_anon_pages_list, label="Guest", color="#4184f3", linewidth=4)

# 添加标题和标签
# plt.title("Host vs Guest AnonPages Over Time")
plt.xlabel("Time (minute)", fontsize=20)
plt.ylabel("AnonPages (GB)", fontsize=20)
plt.xticks([])
plt.tick_params(axis='both', direction='in', length=6)
# 添加图例
plt.legend(fontsize=18)

plt.xticks(fontsize=18)
plt.yticks(fontsize=18)


# 显示网格
plt.grid(True, linestyle='-', alpha=0.7)

# 保存图像到文件 (可选)
plt.savefig("anon_pages_comparison.pdf", dpi=300)

# 显示图形
# plt.show()
