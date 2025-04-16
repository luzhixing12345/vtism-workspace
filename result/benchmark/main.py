import os
import re
import matplotlib.pyplot as plt
import numpy as np

# 设置字体和配色
font_size = 18
axis_font_size = 20
legend_font_size = 18
colors = ['#4c91b8', '#0064ab', '#005083', '#ffc000']
kernels = ["autonuma",  "tpp","nomad", "vtism"]
benchmarks = ["redis", "pr", "graph500", "liblinear", "xsbench", "gups"]


def extract_benchmark_name(cmd_line):
    if "ycsb" in cmd_line:
        return "redis"
    elif "pr" in cmd_line:
        return "pr"
    elif "graph500" in cmd_line:
        return "graph500"
    elif "liblinear" in cmd_line:
        return "liblinear"
    elif "XSBench" in cmd_line:
        return "xsbench"
    elif "gups" in cmd_line:
        return "gups"
    else:
        return None


def parse_benchmark_log(filepath):
    results = {}
    with open(filepath, "r") as f:
        lines = f.readlines()
        i = 0
        while i < len(lines):
            if "[benchmark]" in lines[i]:
                cmd = lines[i + 1].strip()
                time_line = lines[i + 2].strip()
                name = extract_benchmark_name(cmd)
                match_time = re.search(r"time:\s*([\d.]+)", time_line)
                if name and match_time:
                    results[name] = float(match_time.group(1))
                i += 3
            else:
                i += 1
    return results


def load_all_data():
    all_data = {bench: [] for bench in benchmarks}
    for kernel in kernels:
        filepath = f"output/{kernel}-benchmark.log"
        result = parse_benchmark_log(filepath)
        for bench in benchmarks:
            all_data[bench].append(result.get(bench, 0.0))  # 没找到填 0
    return all_data


def plot_benchmark_bar(data_dict):
    x = np.arange(len(benchmarks))
    bar_width = 0.2

    plt.figure(figsize=(10, 6))

    for i, kernel in enumerate(kernels):
        y = []
        for bench in benchmarks:
            base_time = data_dict[bench][0]  # autonuma 的执行时间
            current_time = data_dict[bench][i]
            if current_time > 0:
                norm_speed = (base_time / current_time) * 100
            else:
                norm_speed = 0
            y.append(norm_speed)
            
            # 控制台输出加速比（除 autonuma 外）
            if i > 0:
                delta = norm_speed - 100
                print(f"{kernel:>6} on {bench:<10}: {norm_speed:.2f}% (↑{delta:.2f}%)")

        plt.bar(
            x + i * bar_width,
            y,
            width=bar_width,
            color=colors[i],
            label=kernel,
            edgecolor="black",
            linewidth=1
        )

    plt.xticks(x + 1.5 * bar_width, benchmarks, fontsize=font_size)
    plt.ylabel("Normalized Speed (%)", fontsize=axis_font_size)
    plt.yticks(fontsize=font_size)
    # plt.ylim(60, 200)  # 👉 设置 Y 轴起始为 60
    plt.legend(fontsize=legend_font_size)
    plt.tight_layout()
    plt.savefig("benchmark_normalized.png", dpi=300)
    plt.close()


def main():
    data = load_all_data()
    plot_benchmark_bar(data)


if __name__ == "__main__":
    main()
