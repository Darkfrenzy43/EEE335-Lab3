cmd_/home/user/lab3/task1/Module.symvers := sed 's/\.ko$$/\.o/' /home/user/lab3/task1/modules.order | scripts/mod/modpost -m -a  -o /home/user/lab3/task1/Module.symvers -e -i Module.symvers   -T -
