import re
import matplotlib.pyplot as plt
import numpy as np
import paperplotlib as ppl

def get_data(file_path):
    with open(file_path, "r") as f:
        content = f.read()

    # 正则表达式匹配 A/T 的值
    pattern = re.compile(r"A/T: (\d+)/(\d+)")
    matches = re.findall(pattern, content)

    # 将匹配到的 A 和 T 分别提取出来
    A_values = [int(match[0]) for match in matches]
    T_values = [int(match[1]) for match in matches]
    return A_values, T_values

import sys

def main():
    # 读取数据
    name = sys.argv[1]
    base_name = name + ".log"
    opt_name = name + "_opt.log"
    A_values, T_values = get_data(base_name)
    average_A = sum(A_values) / len(A_values)
    average_T = sum(T_values) / len(T_values)

    opt_A_values, opt_T_values = get_data(opt_name)
    opt_average_A = sum(opt_A_values) / len(opt_A_values)
    opt_average_T = sum(opt_T_values) / len(opt_T_values)

    # 准备数据
    categories = ["walk_page", "walk_page_opt"]
    original_values = [average_A / 1000, opt_average_A / 1000]
    optimized_values = [average_T / 1000, opt_average_T / 1000]

    # 绘制柱状图
    x = np.arange(len(categories))  # x轴的位置
    width = 0.2  # 减小柱宽以减小组间距离

    fig, ax = plt.subplots(figsize=(8, 6))
    #ffc000 #0070c0
    # color_ptes = '#bfbfef'
    # color_time = '#3f3fcf'
    bars1 = ax.bar(
        x - width / 2, original_values, width, label="Accessed PTEs", color="#bfbfef", linewidth=1, edgecolor="black"
    )
    bars2 = ax.bar(
        x + width / 2, optimized_values, width, label="Total Scan PTEs", color="#3f3fcf", linewidth=1, edgecolor="black"
    )
    ax.tick_params(direction='in')

    # 添加标签和标题
    # ax.set_xlabel("Metrics", fontsize=18)
    plt.yticks(fontsize=12)
    ax.set_ylabel("Average PTE count(k)", fontsize=16)
    # ax.set_title("Comparison of Original and Optimized")
    ax.set_xticks(x)
    ax.set_xticklabels(categories, fontsize=16)
    ax.legend(fontsize=16)

    # 添加数值标注
    # for bars in [bars1, bars2]:
    #     for bar in bars:
    #         height = bar.get_height()
    #         ax.annotate(f'{height:.2f}',
    #                     xy=(bar.get_x() + bar.get_width() / 2, height),
    #                     xytext=(0, 3),  # 偏移量
    #                     textcoords="offset points",
    #                     ha='center', va='bottom')

    # 调整布局并保存
    plt.tight_layout()
    # plt.savefig(f"pt_scan_{name}_bar.pdf", dpi=300)
    plt.savefig(f"pt_scan_{name}_bar.png", dpi=300)
    plt.show()


if __name__ == "__main__":
    main()
