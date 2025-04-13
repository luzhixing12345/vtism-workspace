
#!/bin/bash

tmux new-session -d -s qemu
tmux send-keys -t qemu "make" C-m


# 延迟执行 ssh 和 run.sh
tmux new-session -d -s sshvm
tmux send-keys -t sshvm "sleep 5" C-m
tmux send-keys -t sshvm "sshvm" C-m
tmux send-keys -t sshvm "clear" C-m
tmux send-keys -t sshvm "./run.sh $*" C-m

# tmux attach -t sshvm

# tmux capture-pane -t sshvm:0.0 -p 