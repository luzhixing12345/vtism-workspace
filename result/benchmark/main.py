import re
import os
import matplotlib.pyplot as plt
import numpy as np
import paperplotlib as ppl

# 解析 benchmark 结果的函数
def parse_benchmark_output(output):
    results = {}
    
    # 使用正则表达式匹配命令和对应的时间
    pattern = re.compile(r'cmd: (.+?)\ntime: (\d+\.\d+)')
    matches = pattern.findall(output)
    
    # 解析每个匹配项
    for cmd, time in matches:
        key = None
        if "graph500" in cmd:
            key = "graph500"
        elif "ycsb" in cmd:
            key = "redis"
        elif "pr" in cmd:
            key = "pr"
        elif "XSBench" in cmd:
            key = "XSBench"
        
        if key:
            results[key] = float(time)
    
    return results

def plot_results(results_dict):
    benchmarks = ["graph500", "redis", "pr", "XSBench"]
    labels = list(results_dict.keys())  # 内核名称，如不同的日志文件名
    data = [[results_dict[label].get(benchmark, 0) for label in labels] for benchmark in benchmarks]
    graph = ppl.BarGraph()
    graph.plot_2d(data, benchmarks, labels)
    graph.y_label = "Time(s)"
    graph.save('benchmark_results.png')
    # x = np.arange(len(benchmarks))  # 横坐标索引（benchmarks）
    # width = 0.2  # 柱子宽度
    
    # fig, ax = plt.subplots(figsize=(10, 6))
    
    # for i, label in enumerate(labels):
    #     ax.bar(x + i * width, [data[benchmark][i] for benchmark in benchmarks], width, label=label)
    
    # ax.set_xlabel("Benchmarks")
    # ax.set_ylabel("Time (s)")
    # ax.set_title("Benchmark Performance Comparison Across Kernels")
    # ax.set_xticks(x + width * (len(labels) / 2))
    # ax.set_xticklabels(benchmarks)
    
    # plt.tight_layout()
    # # plt.show()
    # plt.savefig('benchmark_results.png')

def main():
    results_dict = {}
    files = os.listdir('output')
    files.sort()
    for file in files:
        with open(f'output/{file}', 'r') as f:
            output = f.read()
            results_dict[file.replace('-benchmark.log', '')] = parse_benchmark_output(output)
            # print(f'{file}: {results_dict[file]}')
    
    plot_results(results_dict)
    
if __name__ == "__main__":
    main()