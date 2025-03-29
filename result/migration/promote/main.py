import re
import matplotlib.pyplot as plt

# 解析日志文件
def parse_numa_log(file_path):
    iterations = []
    node0_pages = []
    node2_pages = []
    
    with open(file_path, 'r') as file:
        iteration = 0
        for line in file:
            match_iter = re.search(r'Random access iteration (\d+)', line)
            match_node0 = re.search(r'NUMA Node 0: \d+ pages \((.+) MB', line)
            match_node2 = re.search(r'NUMA Node 2: \d+ pages \((.+) MB', line)
            
            if match_iter:
                iteration = int(match_iter.group(1))
                iterations.append(iteration)
            elif match_node0:
                node0_pages.append(float(match_node0.group(1)))
            elif match_node2:
                node2_pages.append(float(match_node2.group(1)))
    
    return iterations, node0_pages, node2_pages

# 读取并解析日志
log_file = "vtism.log"  # 替换为你的日志文件路径
iterations, node0, node2 = parse_numa_log(log_file)

# 绘制 NUMA 内存变化曲线
plt.figure(figsize=(10, 5))
plt.plot(iterations, node0, label='NUMA Node 0', marker='o', linestyle='-')
plt.plot(iterations, node2, label='NUMA Node 2', marker='s', linestyle='--')

plt.xlabel("Iteration")
plt.ylabel("Pages Count")
plt.title("NUMA Page Migration Over Time")
plt.legend()
plt.grid(True)
# plt.show()
plt.savefig("numa_migration.png",dpi=300)