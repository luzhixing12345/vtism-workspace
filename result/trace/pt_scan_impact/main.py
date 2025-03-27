import matplotlib.pyplot as plt

# 定义文件名和对应的标签
log_files = {
    "0.log": "baseline",
    "1000.log": "1000 ms",
    "2000.log": "2000 ms",
    "4000.log": "4000 ms",
    "6000.log": "6000 ms",
}

# 颜色和样式
colors = ["#a90226", "#3851a3", "#72aacc", "#fdba6c", "#eb5d3b"]
# line_styles = ["-", "--", "-.", ":", "-"]

plt.figure(figsize=(12, 6))

# 读取并绘制每个日志文件
for i, (file, label) in enumerate(log_files.items()):
    iterations = []
    accesses = []
    
    with open(file, "r") as f:
        for line in f:
            parts = line.strip().split(": ")
            if len(parts) == 2:
                iteration = int(parts[0].split("]")[0][1:])  # 获取 [N] 中的 N
                access = int(parts[1])  # 获取内存访问次数
                iterations.append(iteration)
                accesses.append(access)
    
    plt.plot(iterations, accesses, label=label, color=colors[i], marker="o")

# 设置标题和坐标轴
plt.xlabel("Time(secs)", fontsize=14)
plt.ylabel("Max Memory Accesses (1s)", fontsize=14)
# plt.title("Memory Accesses Over Time with Different Scan Intervals", fontsize=16)

# 添加图例
# plt.legend(title="Scan Interval", fontsize=12)
plt.legend(
    fontsize=12, loc="upper center",
    bbox_to_anchor=(0.5, 1.15), ncol=5
)

# 显示网格
plt.grid(True, linestyle="--", alpha=0.6)

# 保存和显示
plt.tight_layout()
plt.savefig("memory_accesses.png", dpi=300)
# plt.show()
