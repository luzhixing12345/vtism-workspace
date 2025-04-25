import os
import re
import matplotlib.pyplot as plt

def parse_log_file(file_path):
    """解析日志文件，提取两个 traversal 时间并计算 slow down 百分比"""
    with open(file_path, "r") as f:
        content = f.read()
    
    matches = re.findall(r'Sequential traversal completed in (\d+\.\d+) seconds.', content)
    if len(matches) >= 2:
        values = [float(m) for m in matches[:2]]
        slow_down = (values[1] - values[0]) / values[0]
        return int(slow_down * 100)
    return None

def main():
    intervals = ['1000','1500', '2000', '2500']
    file_range = range(1, 25)
    colors = ["#3851a3", "#72aacc", "#fdba6c", "#eb5d3b"]  # 不同间隔对应颜色

    plt.figure(figsize=(10, 6))

    for idx, interval in enumerate(intervals):
        memory_sizes = []
        slowdowns = []
        for i in file_range:
            file_path = os.path.join(interval, f"{interval}_{i}.log")
            if os.path.exists(file_path):
                slowdown = parse_log_file(file_path)
                # print(slowdown)
                if slowdown is not None:
                    memory_sizes.append(i)
                    slowdowns.append(slowdown)
        
        plt.plot(memory_sizes, slowdowns, marker='o', linestyle='-', linewidth=2.5,
                 markersize=5, label=f'{interval} ms', color=colors[idx])
    
    # 坐标轴与图形美化
    plt.xlabel("Active Workload Memory Size (GB)", fontsize=18)
    plt.ylabel("Runtime Slow Down (%)", fontsize=18)
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend(fontsize=14)
    plt.tight_layout()
    plt.savefig("multi_interval_slowdown.png", dpi=300)
    plt.savefig("multi_interval_slowdown.pdf", dpi=300)
    # plt.show()

if __name__ == "__main__":
    main()
