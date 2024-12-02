
import re
import matplotlib.pyplot as plt

def get_data(file_path):
    with open(file_path, "r") as f:
        content = f.read()
        
    # 正则表达式匹配 A/T 的值
    pattern = re.compile(r'Page walk result: A/T: (\d+)/(\d+)')
    matches = re.findall(pattern, content)
    
    # 将匹配到的 A 和 T 分别提取出来
    A_values = [int(match[0])/1000 for match in matches]
    T_values = [int(match[1])/1000 for match in matches]
    # print(len(A_values))
    
    return A_values, T_values

def main():

    A_values, T_values = get_data("vma.txt")
    
    # 将匹配到的 A 和 T 分别提取出来
    opt_A_values, opt_T_values = get_data("vma_clean_opt.txt")
    
    # 绘制曲线
    plt.plot(opt_A_values, label='Acessed PTEs', color='blue')
    plt.plot(T_values, label='Total Page Walks', color='red')
    
    plt.plot(opt_T_values, label='Optimized Page Walks)', color='orange')
    
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
