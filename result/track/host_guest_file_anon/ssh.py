import paramiko
import sys
import subprocess
import json


def ssh_connect_and_execute(hostname, port, username):
    try:
        # 创建 SSH 客户端
        ssh = paramiko.SSHClient()
        # 自动接受主机密钥
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        # 连接到远程主机
        ssh.connect(hostname=hostname, port=port, username=username)

        print(f"Connected to {hostname}")

    except Exception as e:
        print(f"Error: {e}")

    return ssh


def parse_num(result: str):
    results = result.split("\n")
    ans = 0
    for result in results:
        if result == "":
            continue
        ans += int(result)
    return ans


def main():
    # run_redis.sh
    ssh = ssh_connect_and_execute(
        hostname="127.0.0.1",
        port=5555,
        username="kamilu",
    )

    pid = subprocess.check_output("pidof qemu-system-x86_64", shell=True, text=True).strip()

    # 执行命令
    host_anon_cmd = f"cat /proc/{pid}/smaps_rollup" + " | grep Pss_Anon | awk '{print $2}'"
    host_file_cmd = f"cat /proc/{pid}/smaps_rollup" + " | grep Pss_File | awk '{print $2}'"
    guest_anon_cmd = "cat /proc/meminfo | grep '(anon)': | awk '{print $2}'"
    guest_file_cmd = "cat /proc/meminfo | grep '(file)': | awk '{print $2}'"

    host_anon_pages_list = []
    host_file_pages_list = []
    guest_anon_pages_list = []
    guest_file_pages_list = []

    count = 0
    iteration_count = 3000
    while count <= iteration_count:

        # host cmd
        result = subprocess.check_output(host_anon_cmd, shell=True, text=True).strip()
        host_anon_pages = int(result)
        host_anon_pages_list.append(host_anon_pages)

        result = subprocess.check_output(host_file_cmd, shell=True, text=True).strip()
        host_file_pages = int(result)
        host_file_pages_list.append(host_file_pages)

        # guest cmd
        stdin, stdout, stderr = ssh.exec_command(guest_anon_cmd)
        guest_anon_pages = parse_num(stdout.read().decode())
        guest_anon_pages_list.append(guest_anon_pages)

        # guest cmd
        stdin, stdout, stderr = ssh.exec_command(guest_file_cmd)
        guest_file_pages = parse_num(stdout.read().decode())
        guest_file_pages_list.append(guest_file_pages)
        

        count += 1
        if count % 200 == 0:
            print(f"count: {count}")
            print()

    # save as json
    data = {
        "host_anon_pages_list": host_anon_pages_list,
        "host_file_pages_list": host_file_pages_list,
        "guest_anon_pages_list": guest_anon_pages_list,
        "guest_file_pages_list": guest_file_pages_list,
    }

    # with open("data.json", "w") as f:
    #     json.dump(data, f)


if __name__ == "__main__":
    main()
