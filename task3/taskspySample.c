
// First, see if we can get this to build

// --- Importing modules ---    WILL NOT BE USING MACROS FOR NOW

#include <linux/module.h>       // Needed by all modules
#include <linux/kernel.h>       // Needed to get KERN_ALERT, KERN_INFO, etc.

// Adding stuff from lab instructions
#include <linux/proc_fs.h>      // For proc_create() and remove_proc_entry()
#include <linux/seq_file.h>


// --- Defining Module Functions ---

/* 
    Description: Module function called when module inserted into kernel. 

int init_module(void) {

    printk(KERN_ALERT "Alright. this works.\n");


    // Return 0 to indicate init_module successfully ran - module was loaded
    return 0;

} */


/*
    Description: Module function called when module removed from kernel.

void cleanup_module(void) {
    
    printk(KERN_ALERT "Removing the module now.\n");

} */


// ---- Provided code that lets us create and read a special file in /proc ----

// Generates contents of the special file
static int hello_show(struct seq_file *m, void *v) {
    
    seq_printf(m, "Hello My People!\n");
    return 0;

}

// Callback function called when special file opened. 
static int hello_open(struct inode* inode, struct file* file) {
    
    // This has something to do with contents of the special file
    // being generated at once with a sequence file. idk.
    return single_open(file, hello_show, NULL);

}


// Struct that defines the above callback function for handling special file
static const struct proc_ops hello_pops = {
    .proc_open = hello_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Some function that initializes something
static int __init hello_init(void) {

    // Creates the special file in /proc (parent_dir = NULL means it doesn't get put into subdirectory)
    proc_create("hello_file", 0, NULL, &hello_pops);
    return 0;

}


// Some function for exiting 
static void __exit hello_exit(void) {

    // Deletes special file in /proc
    remove_proc_entry("hello_file", NULL);
}




// Module license included to compile the module
MODULE_LICENSE("GPL");

// Calling the module functions
module_init(hello_init);
module_exit(hello_exit);