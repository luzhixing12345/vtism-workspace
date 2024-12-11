import matplotlib.pyplot as plt
import numpy as np


def main():
    # 数据
    graph500 = {0: 210.13, 1000: 210.89, 2000: 210.68, 4000: 210.68, 10000: 376.89}
    liblinear = {0: 376.43, 1000: 426.36, 2000: 398.57, 4000: 177.91, 10000: 376.89}
    xsbench = {0: 79.09, 1000: 203.52, 2000: 191.08, 4000: 88.75, 10000: 376.89}
    redis = {0: 220.13, 1000: 230.16, 2000: 240.10, 4000: 177.91, 10000: 376.89}
    memcached = {0: 210.13, 1000: 215.18, 2000: 225.15, 4000: 177.91, 10000: 376.89}

    benchmarks = ["graph500", "liblinear", "xsbench", "redis", "memcached"]
    time_points = [500, 1000, 2000, 4000]
    data = [graph500, liblinear, xsbench, redis, memcached]

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

    # 依次绘制 3 个时间点的 overhead 数据
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
    ax.set_ylabel("Overhead (%)", fontsize=12)
    ax.set_xticks(x)
    ax.set_xticklabels(benchmarks)
    ax.tick_params(direction="in")
    ax.legend(title="scan interval", fontsize=10)

    # 显示图形
    plt.tight_layout()
    plt.savefig("overhead.pdf", dpi=300)
    plt.show()


if __name__ == "__main__":
    main()
