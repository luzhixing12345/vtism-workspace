import re
import matplotlib.pyplot as plt

# 假设日志文件名是 pt_scan.log
log_file = 'pt_scan_pr.log'

# 用于存储提取的页表值和时间
page_values = []
times = []

# 定义正则表达式，匹配日志中的相关数据
log_pattern = re.compile(r"A/T: (\d+)/(\d+) \((\d+) ")

# 读取日志文件
with open(log_file, 'r') as file:
    for line in file:
        match = log_pattern.search(line)
        if match:
            page_value = int(match.group(2))  # 第二个斜杠后的值
            time_value = int(match.group(3))  # 时间值（微秒）

            page_values.append(page_value)
            times.append(time_value/1000)  # 转换为毫秒

# 统一字体大小变量
FONT_SIZE = 20
LEGEND_SIZE = 20
LABEL_SIZE = 20
TICK_SIZE = 20

# 调整图形尺寸
fig, ax1 = plt.subplots(figsize=(10, 6))

# 绘制页表值的折线
ax1.set_xlabel('Time(minutes)', fontsize=LABEL_SIZE)
ax1.set_ylabel('Scanned PTE items', fontsize=LABEL_SIZE)
ax1.plot(range(len(page_values)), page_values, color='blue', label='Scanned PTE items')
ax1.tick_params(axis='y', labelsize=TICK_SIZE)
ax1.tick_params(axis="both", direction="in", length=6)
# 绘制时间的折线
ax2 = ax1.twinx()
ax2.set_ylabel('Scan Time (ms)', fontsize=LABEL_SIZE)
ax2.plot(range(len(times)), times, color='red', label='Scan Time')
ax2.tick_params(axis='y', labelsize=TICK_SIZE)

# 合并图例，并放置在右上角
fig.legend(loc='upper right', bbox_to_anchor=(0.90, 0.92), ncol=1, fontsize=LEGEND_SIZE)

# 隐藏 x 轴刻度
plt.xticks([], fontsize=TICK_SIZE)
# 调整图形布局
# plt.gca().yaxis.offsetText.set_fontsize(TICK_SIZE-2)

plt.tight_layout()
plt.tick_params(axis="both", direction="in", length=6)

# 保存图像
plt.savefig('pr_scan_time.png', dpi=300)
