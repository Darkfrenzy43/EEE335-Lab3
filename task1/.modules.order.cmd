cmd_/home/user/lab3/task1/modules.order := {   echo /home/user/lab3/task1/hello.ko; :; } | awk '!x[$$0]++' - > /home/user/lab3/task1/modules.order
