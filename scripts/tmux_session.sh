
#!/bin/bash

tmux new-session -d -s qemu
tmux new-session -d -s sshvm

tmux attach -t qemu
tmux attach -t sshvm