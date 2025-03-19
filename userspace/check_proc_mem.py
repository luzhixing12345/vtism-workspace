import psutil
import time
import sys

def get_program_memory_usage(program_name):
    # 获取所有进程
    processes = psutil.process_iter(['pid', 'name', 'memory_info'])

    total_memory = 0

    for proc in processes:
        try:
            # 查找进程名与程序名匹配的进程
            if program_name.lower() in proc.info['name'].lower():
                total_memory += proc.info['memory_info'].rss  # rss: 常驻内存集大小
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            continue

    # 返回内存大小(以GB为单位)
    return total_memory / (1024 ** 3)

def monitor_program_memory(program_name, interval=1):
    max_memory_usage = 0  # 记录最大内存占用
    count = 0
    try:
        while True:
            memory_usage = get_program_memory_usage(program_name)
            max_memory_usage = max(max_memory_usage, memory_usage)  # 更新最大内存占用
            print(f"[{count}] {program_name} 使用的内存: {memory_usage:.2f} GB")
            count += 1
            time.sleep(interval)
    except KeyboardInterrupt:
        # 捕获 Ctrl+C 中断
        print(f"\n最大内存占用: {max_memory_usage:.2f} GB")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("请传入程序名作为参数")
        sys.exit(1)

    program_name = sys.argv[1]
    monitor_program_memory(program_name)
