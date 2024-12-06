import re
import matplotlib.pyplot as plt

def get_data(file_path):
    with open(file_path, "r") as f:
        content = f.read()
        
    # 正则表达式匹配 A/T 的值
    pattern = re.compile(r'vm: (\d+)')
    matches = re.findall(pattern, content)
    
    # 将匹配到的 A 和 T 分别提取出来
    vma = [int(match)/1024  for match in matches]
    # print(len(A_values))
    
    return vma

def main():

    
    gpt_scan = get_data("vma.txt")

    ept_scan = [32 for _ in range(len(gpt_scan))]
    # 绘制曲线
    plt.plot(ept_scan, label="EPT scan", color="blue")
    plt.plot(gpt_scan, label="GPT scan", color="red")

    # 添加标题和标签
    # plt.title('Optimized Page Table Scan')
    plt.xlabel("Time (minute)")
    plt.ylabel("scan vm (GB)")
    plt.xticks([])
    plt.ylim(0, 40)
    plt.tick_params(axis="both", direction="in")
    # 显示图例
    plt.legend()
    plt.savefig("gpt-vs-ept.pdf", dpi=300)
    # plt.savefig('gpt-vs-ept.pdf', dpi=300, pad_inches=0.0, bbox_inches="tight")

    # 显示图形
    # plt.show()


if __name__ == "__main__":
    main()
