
/* 

    Description:

        This module program is to print the pertinent info requested by the lab instruction for
        lab 3 of the course EEE335.



    Status:

        - Create function that converts elapsed time from nano seconds to the required format. 



    Authors: NCdt Eric Cho and OCdt Liethan Velasco

    Version: created on February 19th 2023

*/

// --- Importing modules ---    WILL NOT BE USING MACROS FOR NOW

#include <linux/module.h>       // Needed by all modules
#include <linux/init.h>         // Needed for the module's initialization
#include <linux/kernel.h>       // Needed to get KERN_ALERT, KERN_INFO, etc.
#include <linux/proc_fs.h>      // Needed for proc ops
#include <linux/seq_file.h>     // Needed for writing to sequence file
#include <linux/sched.h>        // Needed for task_struct
#include <linux/sched/signal.h> // Needed for next_task (to get next task in linked list)



/* 
    Description:

        This function only writes all the task names and PIDs of all the current running tasks
        into the special file passed in as <struct seq_file* m>.

*/
// -- Functions that create special file --
static int print_task_info_in_file(struct seq_file* m, void* v) {

    // Getting access to system's task_struct linked list
    struct task_struct* first_task = &init_task;
    struct task_struct* task_walker = first_task;

    // Printing the header line in file
    seq_printf(m, "   PID     PPID     UID       ELAPSED    STATE    NAME\n");

    do {
        
        // Getting the info of the current task
        char* this_name = task_walker -> comm;
        int this_pid = task_walker -> pid;
        int this_ppid = task_walker -> real_parent -> pid;
        kuid_t this_uid = task_walker -> cred -> uid; // <-- Apparently, supoed to be 1000 for new users
        int this_state = task_walker -> __state;

        // convert elapsed time from nano to required format
        unsigned int this_start_time = task_walker -> start_time;
        
        // Writing out the task info formatted
        seq_printf(m, "%7d %8d %7ld %12d %8d   %s\n", 
        this_pid, this_ppid, this_uid, this_start_time, this_state, this_name);

        // Move walker to next task
        task_walker = next_task(task_walker);

    }
    while (task_walker != first_task);
    

    return 0;

}


/*
    Description:

        This function serves as the call back function fo rwhen the special file
        named "task_names_file" is opened.

*/
static int task_info_open(struct inode* inode, struct file* file) {

    return single_open(file, print_task_info_in_file, NULL);

}


// The struct which defines the above callback function for handling the special file
static const struct proc_ops task_info_pops = {
    .proc_open = task_info_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};



// --- Defining Module Functions ---

/* 
    Description: Module function called when module inserted into kernel. 
*/
static int __init run_on_insert(void) {

    printk(KERN_ALERT "Module inserted into kernel.\n");


    // Creating the special file that contains the task names in /proc
    proc_create("task_info_file", 0, NULL, &task_info_pops);

    // Return 0 to indicate function successfully ran - IOW, module was loaded
    return 0;

} 


/*
    Description: Module function called when module removed from kernel.
*/
static void __exit run_on_cleanup(void) {
    
    printk(KERN_ALERT "Removing the module now.\n");

    // Removing the special file from /proc
    remove_proc_entry("task_info_file", NULL);

} 


// Marking the module initializer and exit functions with our custom ones
module_init(run_on_insert);
module_exit(run_on_cleanup);


// Module license included to compile the module
MODULE_LICENSE("GPL");

