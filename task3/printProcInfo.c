
/* 

    Description:

        This module program is to print the pertinent info requested by the lab instruction for
        lab 3 of the course EEE335.



    Status:

        - Successfully was able to create that "special file" in /proc that
        only shows all the processes' PIDs and command names. 

        Will be working on the accessing the remaining information, and getting
        the code formatted next. 



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






// --- Declaring our functions ---
static void print_task_names(void);





// ---- Defining our functions ----

static void print_task_names(void) {

    // Getting access to system's task_struct linked list
    struct task_struct* first_task = &init_task;
    struct task_struct* task_walker = first_task;

    // Printing the PID as a sample for now
    printk(KERN_ALERT "Here is the first task's name: %s\n", first_task -> comm);

    // Getting the second task, printing it's PID
    task_walker = next_task(task_walker);
    printk(KERN_ALERT "Here is the second task's name: %s\n", task_walker -> comm);

    while (task_walker != first_task) {

        printk(KERN_ALERT "Here is task with PID %d's name: %s\n", task_walker -> pid, task_walker -> comm);
        task_walker = next_task(task_walker);

    }


}


// -- Functions that create special file --
static int print_task_names_in_file(struct seq_file* m, void* v) {

    // Getting access to system's task_struct linked list
    struct task_struct* first_task = &init_task;
    struct task_struct* task_walker = first_task;

    // Printing the PID as a sample for now
    seq_printf(m, "Here is the first task's name: %s\n", first_task -> comm);

    // Getting the second task, printing it's PID
    task_walker = next_task(task_walker);
    seq_printf(m, "Here is the second task's name: %s\n", task_walker -> comm);

    while (task_walker != first_task) {

        seq_printf(m, "Here is task with PID %d's name: %s\n", task_walker -> pid, task_walker -> comm);
        task_walker = next_task(task_walker);

    }

    return 0;

}


// -- Creating callback function --
static int task_names_open(struct inode* inode, struct file* file) {

    return single_open(file, print_task_names_in_file, NULL);

}


// - Struct that defines callback function -
static const struct proc_ops task_names_pops = {
    .proc_open = task_names_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};



// --- Defining Module Functions ---

/* 
    Description: Module function called when module inserted into kernel. 
*/
static int __init run_on_insert(void) {

    printk(KERN_ALERT "Alright. this works.\n");

    // creating the special file
    proc_create("task_names_file", 0, NULL, &task_names_pops);


    // Return 0 to indicate function successfully ran - module was loaded
    return 0;

} 


/*
    Description: Module function called when module removed from kernel.
*/
static void __exit run_on_cleanup(void) {
    
    printk(KERN_ALERT "Removing the module now.\n");

    // Removing the special entry
    remove_proc_entry("task_names_file", NULL);

} 




// Marking the module initializer and exit functions with our custom ones
module_init(run_on_insert);
module_exit(run_on_cleanup);



// Module license included to compile the module
MODULE_LICENSE("GPL");

