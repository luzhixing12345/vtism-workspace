import re
import matplotlib.pyplot as plt
import numpy as np

# ---------- 样式参数 ----------
label_fontsize = 20
tick_fontsize = 18
legend_fontsize = 18
bar_label_fontsize = 12
figsize = (10, 6)
dpi = 300
bar_width = 0.35
color_ptes = '#bfbfef'
color_time = '#3f3fcf'

def get_data(file_path):
    with open(file_path, "r") as f:
        content = f.read()
        
    pattern = re.compile(r'A/T: (\d+)/(\d+) \((\d+) ms\)')
    matches = re.findall(pattern, content)
    
    A_values = [int(match[0])  for match in matches]
    T_values = [int(match[1]) for match in matches]
    time_values = [int(match[2]) for match in matches]
    
    return A_values, T_values, time_values

def check_benchmark(name):
    basic_name = name + ".log"
    opt_name = name + "_opt.log"

    A_values, T_values, time_values = get_data(basic_name)
    average_A = sum(A_values) / len(A_values)
    average_T = sum(T_values) / len(T_values)
    average_time = sum(time_values) / len(time_values)
    
    opt_A_values, opt_T_values, opt_time_values = get_data(opt_name)
    opt_average_A = sum(opt_A_values) / len(opt_A_values)
    opt_average_T = sum(opt_T_values) / len(opt_T_values)
    opt_average_time = sum(opt_time_values) / len(opt_time_values)
    
    print(f"{name}")
    print(f"A/T: {average_A:.2f}/{average_T:.2f} ({average_A / average_T * 100:.2f}%)")
    print(f"opt A/T: {opt_average_A:.2f}/{opt_average_T:.2f} ({opt_average_A / opt_average_T * 100:.2f}%)")
    print(f'upgrade T: {(average_T - opt_average_T) / average_T * 100:.2f}%')
    print(f'upgrade time : {opt_average_time:.2f}/{average_time:.2f} ({(average_time - opt_average_time) / average_time * 100:.2f})%')
    
    ptes_speedup = (average_T - opt_average_T) / average_T * 100
    time_speedup = (average_time - opt_average_time) / average_time * 100
    print('----------')
    return ptes_speedup, time_speedup

def plot_speedups(benchmarks, ptes_speedups, time_speedups):
    x = np.arange(len(benchmarks))
    
    fig, ax = plt.subplots(figsize=figsize)

    bars1 = ax.bar(x - bar_width / 2, ptes_speedups, bar_width, label='PTEs Area Reduction',
                   color=color_ptes, linewidth=0.5, edgecolor="black")
    bars2 = ax.bar(x + bar_width / 2, time_speedups, bar_width, label='Scan Time Reduction',
                   color=color_time, linewidth=0.5, edgecolor="black")

    ax.tick_params(direction='in')
    ax.set_ylabel('Reduction (%)', fontsize=label_fontsize)
    ax.set_xticks(x)
    ax.set_xticklabels(benchmarks, fontsize=tick_fontsize)
    plt.yticks(fontsize=tick_fontsize)
    ax.legend(fontsize=legend_fontsize)

    def add_labels(bars):
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width() / 2, height + 0.5,
                    f'{height:.2f}', ha='center', va='bottom', fontsize=bar_label_fontsize)

    # add_labels(bars1)
    # add_labels(bars2)

def main():
    benchmarks = ['redis', "xsbench", 'pr', 'graph500', 'gups', 'liblinear']
    ptes_speedups = []
    time_speedups = []

    for benchmark in benchmarks:
        ptes_speedup, time_speedup = check_benchmark(benchmark)
        ptes_speedups.append(ptes_speedup)
        time_speedups.append(time_speedup)

    plot_speedups(benchmarks, ptes_speedups, time_speedups)
    plt.tight_layout()
    plt.savefig('pt_scan_opt_all.png', dpi=dpi)

if __name__ == "__main__":
    main()
