import matplotlib.pyplot as plt
import numpy as np


def main():
    # 数据
    graph500 = {0: 208.98, 1000: 209.99, 2000: 209.82, 5000: 209.6, 10000: 209.1}
    liblinear = {0: 375.70, 1000: 432.51, 2000: 396.80, 5000: 376.99, 10000: 376.89}
    # pr = {0: 238.19, 1000: 323.79, 2000: 279.00, 5000: 261.67, 10000: 248.42}
    xsbench = {0: 40.91, 1000: 46.76, 2000: 46.10, 5000: 42.82, 10000: 41.85}
    redis = {0: 213.46, 1000: 231.11, 2000: 224.43, 5000: 219.25, 10000: 218.23}
    memcached = {0: 77.14, 1000: 78.87, 2000: 78.66, 5000: 78.36, 10000:  78.16}

    time_points = [1000, 2000, 5000, 10000]
    data = [graph500, liblinear, xsbench, redis, memcached]
    benchmarks = ["graph500", "liblinear", "xsbench", "redis", "memcached"]

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

    # 纵轴 5 的位置画一条横线
    ax.axhline(5, color="black", linestyle="--", linewidth=0.5)

    # 显示图形
    plt.tight_layout()
    plt.savefig("overhead.pdf", dpi=300)
    plt.show()


if __name__ == "__main__":
    main()
