#!/bin/bash

# 关闭名为 qemu 和 sshvm 的 tmux 会话
tmux kill-session -t qemu 2>/dev/null
tmux kill-session -t sshvm 2>/dev/null