
import re
import matplotlib.pyplot as plt
import numpy as np

def get_data(file_path):
    with open(file_path, "r") as f:
        content = f.read()
        
    # 正则表达式匹配 A/T 的值
    pattern = re.compile(r'pt_scan: A/T: (\d+)/(\d+) \((\d+) ms\)')
    matches = re.findall(pattern, content)
    
    # 将匹配到的 A 和 T 分别提取出来
    A_values = [int(match[0])  for match in matches]
    T_values = [int(match[1]) for match in matches]
    time_values = [int(match[2]) for match in matches]
    # print(len(A_values))
    
    return A_values, T_values, time_values

def main():

    import sys
    name = sys.argv[1]
    basic_name = name + ".log"
    opt_name = name + "_opt.log"

    A_values, T_values, time_values = get_data(basic_name)
    average_A = sum(A_values) / len(A_values)
    average_T = sum(T_values) / len(T_values)
    average_time = sum(time_values) / len(time_values)
    
    # 将匹配到的 A 和 T 分别提取出来
    opt_A_values, opt_T_values, opt_time_values = get_data(opt_name)
    opt_average_A = sum(opt_A_values) / len(opt_A_values)
    opt_average_T = sum(opt_T_values) / len(opt_T_values)
    opt_average_time = sum(opt_time_values) / len(opt_time_values)
    
    # print("Accessed PTEs:", average_A)
    # print("Total Page Walks:", average_T)
    # print("Time:", average_time)
    # print("Opt Acessed PTEs:", opt_average_A)
    # print("Opt Page Walks:", opt_average_T)
    # print("Opt Time:", opt_average_time)
    print(f"{name}")
    print(f"A/T: {average_A:.2f}/{average_T:.2f} ({average_A / average_T * 100:.2f}%)")
    print(f"opt A/T: {opt_average_A:.2f}/{opt_average_T:.2f} ({opt_average_A / opt_average_T * 100:.2f}%)")
    
    print(f'upgrade T: {(average_T - opt_average_T) / average_T * 100:.2f}%')
    print(f'upgrade time : {opt_average_time:.2f}/{average_time:.2f} ({(average_time - opt_average_time) / average_time * 100:.2f})%')
    
    # ptes_speedup = (average_T - opt_average_T) / average_T * 100
    # time_speedup = (average_time - opt_average_time) / average_time * 100

    # # 绘制曲线
    plt.plot(A_values, label='Acessed PTEs', color='green')
    plt.plot(T_values, label='Total Page Walks', color='red')
    plt.plot(opt_A_values, label='Opt Acessed PTEs', color='blue')
    plt.plot(opt_T_values, label='Opt Page Walks)', color='orange')
    
    # 添加标题和标签
    # plt.title('Optimized Page Table Scan')
    plt.xlabel('Time (minute)')
    plt.ylabel('Count(k)')
    
    # 显示图例
    plt.legend()
    plt.savefig(f'pt_scan_{name}.png', dpi=300)
    print(f"generate {f'pt_scan_{name}.png'}")
    # plt.savefig('pt_scan.pdf', dpi=300, pad_inches=0.0, bbox_inches="tight")
    

if __name__ == "__main__":
    main()
