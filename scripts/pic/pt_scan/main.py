
import re
import matplotlib.pyplot as plt

def get_data(file_path):
    with open(file_path, "r") as f:
        content = f.read()
        
    # 正则表达式匹配 A/T 的值
    pattern = re.compile(r'Page walk result: A/T: (\d+)/(\d+)')
    matches = re.findall(pattern, content)
    
    # 将匹配到的 A 和 T 分别提取出来
    A_values = [int(match[0])  for match in matches]
    T_values = [int(match[1]) for match in matches]
    # print(len(A_values))
    
    return A_values, T_values

def main():

    name = "xsbench"
    basic_name = name + ".log"
    opt_name = name + "_opt.log"

    A_values, T_values = get_data(basic_name)
    average_A = sum(A_values) / len(A_values)
    average_T = sum(T_values) / len(T_values)
    
    # 将匹配到的 A 和 T 分别提取出来
    opt_A_values, opt_T_values = get_data(opt_name)
    opt_average_A = sum(opt_A_values) / len(opt_A_values)
    opt_average_T = sum(opt_T_values) / len(opt_T_values)
    
    print("average A: ", average_A)
    print("average T: ", average_T)
    print("average opt A: ", opt_average_A)
    print("average opt T: ", opt_average_T)
    
    # 绘制曲线
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
    plt.savefig('pt_scan.pdf', dpi=300)
    # plt.savefig('pt_scan.pdf', dpi=300, pad_inches=0.0, bbox_inches="tight")
    
    # 显示图形
    # plt.show()

if __name__ == "__main__":
    main()
