
/* 

    Description:

        This module program is to print the pertinent info of currently running tasks as detailed by 
        the lab instruction of lab 3 of the course EEE335.



    Notes:

        1 - The ELAPSED time of a process is the time between starting the operating system and 
        the creation of the given process. It follows that the more recent a process is created,
        the longer its ELAPSED time would be. 



    Status:


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



// --- Defining MACROS ---

#define DAY_NS  86400000000000
#define HOUR_NS 3600000000000
#define MIN_NS  60000000000
#define SEC_NS  1000000000



// --- Declaring functions ---

void convert_nanotime(unsigned long int in_time, int equiv_time[]);
char get_state(unsigned int task_state);



// --- Defining functions ---

/* 
    Description:

        This function only writes all the task names and PIDs of all the current running tasks
        into the special file passed in as <struct seq_file* m>.

*/
static int print_task_info_in_file(struct seq_file* m, void* v) {

    // Getting access to system's task_struct linked list
    struct task_struct* first_task = &init_task;
    struct task_struct* task_walker = first_task;

    // Printing the header line in file
    seq_printf(m, "    PID     PPID     UID            COMMAND      ELAPSED  STATE\n");

    // Creating array to store equivalent days, hours, minutes, and seconds
    int equiv_time[4];


    do {
        
        // Getting the info of the current task
        char* this_name = task_walker -> comm;
        int this_pid = task_walker -> pid;
        int this_ppid = task_walker -> parent -> pid;
        kuid_t this_uid = task_walker -> cred -> uid; // <-- Apparently, supoed to be 1000 for new users
        
        // Obtain the task's state, and the letter equivalent
        unsigned int state_number = task_walker -> __state;
        char state_letter = get_state(state_number);

        // convert elapsed time from nano to required format
        unsigned long int this_start_time = task_walker -> start_time;
        convert_nanotime(this_start_time, equiv_time);
        
        // Writing out the task info formatted
        seq_printf(m, "%7d %8d %7ld %18s %3d-%02d:%02d:%02d %6c   \n", 
        this_pid, this_ppid, this_uid, this_name, equiv_time[0], equiv_time[1], equiv_time[2], equiv_time[3], state_letter);

        
        // Move walker to next task
        task_walker = next_task(task_walker);

    }
    while (task_walker != first_task);
    

    return 0;

}


/* 
    Description:

        This function takes in an task_state as its enumerated value, and
        returns the corresponding character as what would be seen in the STATE column after running ps.

        (R) TASK_RUNNING = 0 
        (S) TASK_INTERRUPTIBLE = 1 
        (D) TASK_UNINTERRUPTIBLE = 2 
        (T) TASK_STOPPED = 260   
        (I) TASK_IDLE = 1026 

    Arguments:

        <unsigned int task_state> : The task state extracted from the task_struct of a process.

    Returns:
        The corresponding character of the inputted task_state. Returns null if no corresponding state found. 

*/
char get_state(unsigned int task_state) {

    // Use switch statement
    switch (task_state){

        case TASK_RUNNING:
            return 'R';
            break;
        case TASK_INTERRUPTIBLE:
            return 'S';
            break;
        case TASK_UNINTERRUPTIBLE:
            return 'D';
            break;
        case TASK_STOPPED:
            return 'T';
            break;
        case TASK_IDLE:
            return 'I';
            break;
        default:
            return '\0';
    }

}


/* 
    Description:

        This function converts nano seconds into its equivalent time in seconds,
        minutes, hours, and days, in the format of [[dd-]hh:]mm:ss, where fields
        in the square brackets are optional.

    Arguments:
        
        <unsigned long int : in_time> : The time in nano seconds to be converted to the desired format.
        
        <int equiv_time[]> : Pointer to passed in array of size 4 to be loaded with the calculated equivalent time. 
                                (0th index - days, 1st index - hours, 2nd index - minutes, 3rd index - seconds)

*/
void convert_nanotime(unsigned long int in_time, int equiv_time[]) {

    // Declaring variables to be used in conversion
    int days;
    int hours;
    int minutes;
    int seconds;

    
    // Use dummy_var that will be used to compute equivalent time
    unsigned long int remaining_time = in_time;

    // Find how many days, hours, minutes, and seconds in_time is equivalent to.
    // NOTE: Rounding down is already done by implicit conversion from float to int
    days = remaining_time / DAY_NS;
    remaining_time -= days * DAY_NS;
    hours = remaining_time / HOUR_NS;
    remaining_time -= hours * HOUR_NS;
    minutes = remaining_time / MIN_NS;
    remaining_time -= minutes * MIN_NS;
    seconds = remaining_time / SEC_NS;

    // Loading calculated times into equiv_time array
    equiv_time[0] = days;
    equiv_time[1] = hours;
    equiv_time[2] = minutes;
    equiv_time[3] = seconds;

}


/*
    Description:

        This function serves as the call back function for when the special file
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


// Marking the module initializer and exit functions with macros
module_init(run_on_insert);
module_exit(run_on_cleanup);


// Module license included to compile the module
MODULE_LICENSE("GPL");

