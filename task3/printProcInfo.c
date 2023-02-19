
/* 

    Description:

        This module program is to print the pertinent info requested by the lab instruction for
        lab 3 of the course EEE335.



    Status:

        - Begin implementing the program that prints the the requested information into
        the special file in /proc. Refer to lab instructions and Notions for info to help. 



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


// --- Defining Module Functions ---

/* 
    Description: Module function called when module inserted into kernel. 
*/
int init_module(void) {

    printk(KERN_ALERT "Alright. this works.\n");


    // Return 0 to indicate init_module successfully ran - module was loaded
    return 0;

} 


/*
    Description: Module function called when module removed from kernel.
*/
void cleanup_module(void) {
    
    printk(KERN_ALERT "Removing the module now.\n");

} 






// Module license included to compile the module
MODULE_LICENSE("GPL");

