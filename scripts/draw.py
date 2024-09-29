
import paperplotlib as ppl
import os
import re
from typing import Dict, List

OUTPUT_DIR="output"
BASELINE_KERNEL = "baseline"

def parse_log():
    # {version/.}{name}-benchmark.log
    pattern = r"([\d\.]+)(\w+)\+?-benchmark\.log"
    results = {}
    
    for file in os.listdir(OUTPUT_DIR):
        kernel_name = re.match(pattern, file).group(2)
        print(f"load {kernel_name} log")
        results[kernel_name] = {}
        with open(os.path.join(OUTPUT_DIR, file), "r") as f:
            lines = f.readlines()
            # [benchmark_name]: time
            for line in lines:
                log_pattern = r"\[(\w+)\]: (\d+\.\d+)"
                match = re.match(log_pattern, line)
                if match:
                    benchmark_name = name_filter(match.group(1))
                    time = float(match.group(2))
                    # print("find log: ", name, time)
                    results[kernel_name][benchmark_name] = time
                else:
                    print("log format error, should be [benchmark_name]: time")
    
    
    if BASELINE_KERNEL in results:
        results = normalize(results)
        print("normalize done")
    else:
        print("baseline log not found, use original data")
    
    return results

def normalize(results: Dict[str, Dict[str, str]]):
    
    normalized_results = {}
    base_line_data = results[BASELINE_KERNEL]
    for k, v in results.items():
        if k == BASELINE_KERNEL:
            continue
        normalized_results[k] = {}
        for benchmark_name, time in v.items():
            # save 2 decimal
            normalized_results[k][benchmark_name] = round(time / base_line_data[benchmark_name], 2)
    
    # baseline data -> 1
    normalized_results[BASELINE_KERNEL] = {}
    for benchmark_name, time in base_line_data.items():
        normalized_results[BASELINE_KERNEL][benchmark_name] = 1
    
    return normalized_results

def name_filter(name):
    name_dict = {
        'graph500_reference_bfs': 'bfs'
    }
    
    if name in name_dict:
        return name_dict[name]
    else:
        return name


def draw(results: Dict[str, Dict[str, str]]):
    
    # 初始化一个对象
    graph = ppl.BarGraph()

    # 传入数据/组/列的文字信息
    group_names = []
    column_names = []
    for k, v in results.items():
        column_names.append(k)
    
    for name, time in results[column_names[0]].items():
        group_names.append(name)
    
    data = []
    for group_name in group_names:
        group = []
        for column_name in column_names:
            group.append(results[column_name][group_name])
        data.append(group)
    
    graph.plot_2d(data, group_names, column_names)

    # 调整x/y轴文字
    graph.x_label = "The number of data"
    graph.y_label = "Throughput (Mbps)"

    # 保存图片
    graph.save()
    

if __name__ == "__main__":
    results = parse_log()
    draw(results)