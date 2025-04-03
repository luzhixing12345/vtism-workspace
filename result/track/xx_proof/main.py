import os
import re
import matplotlib.pyplot as plt
import numpy as np

def parse_log_file(file_path):
    """解析日志文件，提取 Max memory accesses 并计算平均值"""
    with open(file_path, "r") as f:
        content = f.read()
    
    matches = re.findall(r'Traversal completed in (\d+\.\d+) seconds.', content)
    values = [float(m) for m in matches]
    slow_down = (values[1] ) / values[0]
    return slow_down

def main():
    log_dir = "."  # 日志所在目录，当前目录
    memory_sizes = []
    avg_accesses = []
    
    # 遍历 1.log 到 32.log
    for i in range(1, 9):
        file_name = f"{i}.log"
        file_path = os.path.join(log_dir, file_name)
        
        if os.path.exists(file_path):
            avg_value = parse_log_file(file_path)
            if avg_value is not None:
                memory_sizes.append(i)
                avg_accesses.append(avg_value)
    
    # 绘制折线图
    plt.figure(figsize=(10, 6))
    plt.plot(memory_sizes, avg_accesses, marker='o', linestyle='-', linewidth=2.5, markersize=5, color='blue')
    
    # 设置轴标签和标题
    plt.xlabel("Memory Size (GB)", fontsize=18)
    plt.ylabel("Avg Max Memory Accesses (per sec)", fontsize=18)
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)
    plt.grid(True, linestyle="--", alpha=0.6)
    
    # 显示图表
    plt.tight_layout()
    plt.savefig("memory_access_trend.png", dpi=300)
    # plt.show()

if __name__ == "__main__":
    main()
