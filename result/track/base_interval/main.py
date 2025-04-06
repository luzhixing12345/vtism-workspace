import re
import matplotlib.pyplot as plt

# ========== 可调参数 ==========
log_file = "base_interval.log"  # 日志文件路径
bar_color = "blue"  # 柱子颜色
bar_edge = "black"  # 柱子边缘颜色
font_size = 14  # 字体大小
ylabel_fontsize = 16  # y 轴标签字体大小
xlabel_fontsize = 16  # x 轴标签字体大小
title_fontsize = 18  # 标题字体大小
# ==============================

with open(log_file, 'r') as f:
    content = f.read()

# 拆分为每个 interval 段
segments = re.split(r"(\d+)\s+interval", content)

intervals = []
overheads = []

# 每两个 segments 是 [interval, content]
for i in range(1, len(segments), 2):
    interval = segments[i].strip()
    segment = segments[i + 1]

    # 匹配两次遍历时间
    times = re.findall(r"Sequential traversal completed in ([\d.]+) seconds", segment)
    if len(times) >= 2:
        t1 = float(times[0])
        t2 = float(times[1])
        overhead = (t2 - t1) / t1 * 100  # 计算 overhead
        intervals.append(interval)
        overheads.append(overhead)
    else:
        print(f"Warning: Interval {interval} does not contain enough time entries.")

# 绘制柱状图
plt.figure(figsize=(4, 6))
x = range(len(intervals))
bars = plt.bar(x, overheads, color=bar_color, edgecolor=bar_edge)
plt.axhline(y=5, color='grey', linestyle='--', linewidth=2)
# 设置x轴刻度
plt.xticks(x, intervals, fontsize=font_size)

# 设置y轴刻度
plt.yticks([0,2,4,5,6,8,10], fontsize=font_size)

# 设置标签
plt.ylabel("Overhead (%)", fontsize=ylabel_fontsize)
plt.xlabel("Base Scan Interval Time(ms)", fontsize=xlabel_fontsize)

# 设置标题
# plt.title("Traversal Overhead vs Scan Interval", fontsize=title_fontsize)

# 添加网格
plt.grid(axis='y', linestyle='--', alpha=0.5)

# 调整布局，确保标签显示完整
plt.tight_layout()

# 保存图像
plt.savefig("base_scan_interval.png", dpi=300)

# 显示图像（如果需要，可以取消注释下面一行）
# plt.show()
