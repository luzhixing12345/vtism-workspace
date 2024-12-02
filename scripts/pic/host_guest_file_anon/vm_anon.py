import json
import matplotlib.pyplot as plt
from matplotlib import rc

# import matplotlib.font_manager as fm
# fm.fontManager.addfont('consola-1.ttf')
# plt.rcParams["font.family"] = "Consolas"

# 加载 JSON 数据
with open("data.json", "r") as f:
    data = json.load(f)

host_anon_pages_list = data["host_anon_pages_list"][:-600]
host_file_pages_list = data["host_file_pages_list"][:-600]
guest_anon_pages_list = data["guest_anon_pages_list"][:-600]
guest_file_pages_list = data["guest_file_pages_list"][:-600]

# for i in range(len(host_anon_pages_list)):
#     if host_anon_pages_list[i] < 1737192:
#         print(i, host_anon_pages_list[i])

# exit()

for i in range(len(host_anon_pages_list)):
    host_anon_pages_list[i] = host_anon_pages_list[i] / 1024 / 1024
    host_file_pages_list[i] = host_file_pages_list[i] / 1024 / 1024
    guest_anon_pages_list[i] = guest_anon_pages_list[i] / 1024 / 1024
    guest_file_pages_list[i] = guest_file_pages_list[i] / 1024 / 1024

# 创建 X 轴 (数据点的索引)
x = list(range(len(host_anon_pages_list)))

# 绘制图形
plt.figure(figsize=(10, 6))
# https://blog.csdn.net/CD_Don/article/details/88070453
# 绘制宿主机数据
plt.plot(x, host_anon_pages_list, label="Host VM Anon", color="red", linewidth=4)
plt.plot(x, host_file_pages_list, label="Host VM File", color="mediumturquoise", linewidth=4)

# 4184f3

# 绘制虚拟机数据
plt.plot(x, guest_anon_pages_list, label="Guest Anon", color="orange", linewidth=4)

# 绘制虚拟机数据
# 浅蓝色
plt.plot(x, guest_file_pages_list, label="Guest File", color="blue", linewidth=4)

# 添加标题和标签
# plt.title("Host vs Guest AnonPages Over Time")
plt.xlabel("Time (minute)", fontsize=20)
plt.ylabel("Pages (GB)", fontsize=20)
plt.xticks([])
plt.tick_params(axis="both", direction="in", length=6)
# 添加图例
plt.legend(fontsize=15)

plt.xticks(fontsize=18)
plt.yticks(fontsize=18)


# 显示网格
plt.grid(True, linestyle="-", alpha=0.7)
plt.tight_layout()
# 保存图像到文件 (可选)
# plt.savefig("anon_pages_comparison.png", dpi=300, pad_inches=0.0, bbox_inches="tight")
plt.savefig("anon_pages_comparison.pdf", dpi=300)
# plt.savefig("anon_pages_comparison.pdf", dpi=300, pad_inches=0.0, bbox_inches="tight")

# 显示图形
# plt.show()
