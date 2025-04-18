import re
import matplotlib.pyplot as plt

# 可调整的字体大小
axis_font_size = 22
tick_font_size = 22
legend_font_size = 18
def get_data(file_path):
    with open(file_path, "r") as f:
        content = f.read()
        
    # 正则表达式匹配 A/T 的值
    pattern = re.compile(r'vm: (\d+)')
    matches = re.findall(pattern, content)
    
    # 将匹配到的 A 和 T 分别提取出来
    vma = [int(match)/1024 for match in matches]
    
    return vma

def main():
    gpt_scan = get_data("vma.txt")

    ept_scan = [32 for _ in range(len(gpt_scan))]
    
    plt.figure(figsize=(10, 6))
    # 绘制曲线
    plt.plot(ept_scan, label="HPT scan", color="blue", linewidth=2)
    plt.plot(gpt_scan, label="GPT scan", color="red", linewidth=2)

    # 添加标题和标签
    plt.xlabel("Time (minutes)", fontsize=axis_font_size)
    plt.ylabel("scan vm size (GB)", fontsize=axis_font_size)
    
    # 调整刻度字体大小
    plt.xticks(fontsize=tick_font_size)
    plt.yticks(fontsize=tick_font_size)

    # 隐藏 X 轴刻度
    plt.xticks([])
    plt.ylim(0, 40)
    plt.tick_params(axis="both", direction="in")

    # 显示图例
    plt.legend(fontsize=legend_font_size)
    plt.tight_layout()

    # 保存图像
    plt.savefig("gpt-vs-hpt.png", dpi=300)

if __name__ == "__main__":
    main()
