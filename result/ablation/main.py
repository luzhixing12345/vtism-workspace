import re
import matplotlib.pyplot as plt
import numpy as np

# 设置样式参数
font_size = 18
axis_font_size = 18
colors = ["#3851a3", "#72aacc", "#fdba6c", "#eb5d3b"]
labels = [
    'autonuma + sync migration',
    'autonuma + fix threshold + sync migration',
    'autonuma + adaptive scan + sync migration',
    'autonuma + adaptive scan + async migration'
]
benchmarks = ['graph500','pr']

def parse_benchmark_log(file_path):
    result = {}
    current_cmd = ""
    with open(file_path, 'r') as f:
        for line in f:
            cmd_match = re.search(r'cmd: (.+)', line)
            time_match = re.search(r'time: ([\d.]+)', line)
            if cmd_match:
                current_cmd = cmd_match.group(1)
            elif time_match and current_cmd:
                for bench in benchmarks:
                    if bench in current_cmd:
                        result[bench] = float(time_match.group(1))
    return result

# 文件路径
files = ['autonuma.log', '+static.log', '+adaptive.log','all.log']
raw_data = [parse_benchmark_log(f) for f in files]

# 归一化运行时间为 speed（base = 100%）
normalized_data = []
for i in range(len(raw_data)):
    norm_speed = []
    for bench in benchmarks:
        base_time = raw_data[0][bench]
        current_time = raw_data[i].get(bench, base_time)
        speed = base_time / current_time * 100
        norm_speed.append(base_time / current_time * 100)
        if i > 0:
            print(f"{labels[i]} on {bench}: {speed:.2f}% (↑{speed - 100:.2f}%)")
    normalized_data.append(norm_speed)

# 准备绘图数据
bar_width = 0.2
x = np.arange(len(benchmarks))

plt.figure(figsize=(10, 6))

# 画柱状图
for i in range(len(normalized_data)):
    plt.bar(
        x + i * bar_width,
        normalized_data[i],
        width=bar_width,
        color=colors[i],
        label=labels[i],
        edgecolor='black',
        linewidth=1
    )

# 设置轴标签和图例
plt.xticks(x + 1.5 * bar_width, benchmarks, fontsize=font_size)
plt.ylabel("Normalized Speed (%)", fontsize=axis_font_size)
plt.yticks(fontsize=font_size)
plt.ylim(60, 150)
plt.legend(fontsize=font_size)
plt.tight_layout()
plt.savefig("ablation_speed.png", dpi=300)
plt.close()
