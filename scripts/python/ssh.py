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


def main():
    # run_redis.sh
    ssh = ssh_connect_and_execute(
        hostname="127.0.0.1",
        port=5555,
        username="kamilu",
    )

    # pid = int(sys.argv[1])
    pid = subprocess.check_output("pidof qemu-system-x86_64", shell=True, text=True).strip()
    
    # 执行命令
    host_cmd = f"cat /proc/{pid}/status" + " | grep RssAnon | awk '{print $2}'"
    guest_cmd = "cat /proc/meminfo | grep AnonPages | awk '{print $2}'"
    
    host_anon_pages_list = []
    guest_anon_pages_list = []

    count = 0
    while count <= 5000:
        
        # host cmd
        result = subprocess.check_output(host_cmd, shell=True, text=True).strip()
        host_anon_pages = int(result)
        host_anon_pages_list.append(host_anon_pages)
        
        # guest cmd
        stdin, stdout, stderr = ssh.exec_command(guest_cmd)
        guest_anon_pages = int(stdout.read().decode())
        guest_anon_pages_list.append(guest_anon_pages)
        
        count += 1
        if count % 200 == 0:
            print(f"count: {count}")

    # save as json
    data = {
        "host_anon_pages_list": host_anon_pages_list,
        "guest_anon_pages_list": guest_anon_pages_list,
    }

    with open("data.json", "w") as f:
        json.dump(data, f)

if __name__ == "__main__":
    main()
