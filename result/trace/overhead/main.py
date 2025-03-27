import matplotlib.pyplot as plt
import numpy as np

# 字体大小配置
LABEL_FONT_SIZE = 14
TICK_FONT_SIZE = 16
LEGEND_FONT_SIZE = 14
TITLE_FONT_SIZE = 16
AXIS_LABEL_FONT_SIZE = 16

def main():
    # 数据
    graph500 = {0: 208.98, 500: 210.78, 1000: 209.99, 2000: 209.82, 4000: 209.6}
    pr = {0: 195.50, 500: 202.93, 1000: 200.65, 2000: 199.4, 4000: 198}
    xsbench = {0: 96.42, 500: 99.06, 1000: 98.9, 2000: 98.19, 4000: 97.5}
    redis = {0: 211.40, 500: 224.64, 1000: 222.28, 2000: 218.99, 4000: 216.12}

    time_points = [500, 1000, 2000, 4000]
    data = [graph500, pr, xsbench, redis]
    benchmarks = ["graph500", "pr", "xsbench", "redis"]

    # 计算 overhead
    overhead_data = []
    for benchmark_data in data:
        base = benchmark_data[0]
        overhead = {t: (benchmark_data[t] - base) / base * 100 if base != 0 else 0 for t in time_points}
        overhead_data.append(overhead)

    # 设置绘图参数
    x = np.arange(len(benchmarks))  # 每组柱子的 x 坐标
    width = 0.2  # 每个柱子的宽度
    offsets = [-1.5, -0.5, 0.5, 1.5]  # 4 个时间点的偏移量

    fig, ax = plt.subplots(figsize=(10, 6))
    colors = ["#ccccf2", "#9999e5", "#6666d8", "#3333cb"]

    # 依次绘制不同时间点的 overhead 数据
    for i, time in enumerate(time_points):
        heights = [d[time] for d in overhead_data]
        ax.bar(
            x + offsets[i] * width,
            heights,
            width,
            label=f"{time} ms",
            color=colors[i],
            linewidth=0.5,
            edgecolor="black",
        )

    # 设置标题和轴标签
    ax.set_ylabel("Overhead (%)", fontsize=AXIS_LABEL_FONT_SIZE)
    ax.set_xticks(x)
    ax.set_xticklabels(benchmarks, fontsize=TICK_FONT_SIZE)
    ax.tick_params(axis="both", labelsize=TICK_FONT_SIZE, direction="in")
    ax.legend(title="scan base interval", fontsize=LEGEND_FONT_SIZE, title_fontsize=LABEL_FONT_SIZE)

    # 纵轴 5 的位置画一条横线
    ax.axhline(5, color="black", linestyle="--", linewidth=0.5)
    
    # y 轴范围 0-10
    ax.set_ylim(0, 10)

    # 显示图形
    plt.tight_layout()
    plt.savefig("overhead.pdf", dpi=300)
    plt.savefig("overhead.png", dpi=300)
    # plt.show()


if __name__ == "__main__":
    main()
