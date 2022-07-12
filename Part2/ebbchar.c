#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/semaphore.h>
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/mutex.h>

#define  DEVICE_NAME "ebbchar"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "ebb"  
int account[100];

static struct mutex lock[100];

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Derek Molloy");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the BBB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};


static int __init ebbchar_init(void){
   printk(KERN_INFO "EBBChar: Initializing the EBBChar LKM\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "EBBChar failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "EBBChar: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ebbcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ebbcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "EBBChar: device class registered correctly\n");

   // Register the device driver
   ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ebbcharDevice)){               // Clean up if there is an error
      class_destroy(ebbcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ebbcharDevice);
   }
   printk(KERN_INFO "EBBChar: device class created correctly\n"); // Made it! device was initialized
   
   int i;
   for(i=0;i<100;i++){
   account[i]=2000000;
   mutex_init(&lock[i]);
   }
   
   return 0;
}


static void __exit ebbchar_exit(void){
   device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "EBBChar: Goodbye from the LKM!\n");
}


static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   int i;
      for(i=0;i<100;i++){
        printk(KERN_CONT "%d,", account[i]);
      }
      printk(KERN_INFO "\n");
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   	int error_count = 0,from=0,to=0,value=0,i=2;;
        char message[100];
        error_count = copy_from_user(message, buffer, len); 
        
        if (error_count!=0){
	printk(KERN_INFO "SendChar: Failed to receive characters from the user\n");
	return -EFAULT;
        }       

       while (message[i] != 44) {
           if (message[i] != 45) {
              from = from * 10 + (message[i] - 48);
         } else {
             from = -1;
        }
          i++;
      }
        i++;
       while (message[i] != 44) {
          if (message[i] != 45) {
            to = to * 10 + (message[i] - 48);
         } else {
             to = -1;
        } 
         i++;
      } 
       i++;

     while (i<len) {
        if (message[i] != 45) {
            value = value * 10 + (message[i] - 48);
        } else {
            to = -1;
        }
        i++;
      } 
   
if(message[0]==101 && message[1]==44)//e
    {
       if(from>=0&&to>=0){

          if(to<from){
               mutex_lock(&lock[from]);
               mutex_lock(&lock[to]);
          }
          else{
               mutex_lock(&lock[to]);
               mutex_lock(&lock[from]);
          }
        //if(account[from]>=value){  
             account[from]-=value;
             account[to]+=value;
       // }
      //  else{
       //   printk(KERN_INFO "Source doesn't have enough money\n");
       //  }
          if(to<from){
               mutex_unlock(&lock[from]);
               mutex_unlock(&lock[to]);
          }
          else{
               mutex_unlock(&lock[to]);
               mutex_unlock(&lock[from]);
          }
       }else{
       printk(KERN_INFO "Invalid input\n");
       }
    }   
    else if(message[0]==118&& message[1]==44)//v
    {
     if(from==-1&&to>=0){
          mutex_lock(&lock[to]);
          account[to]+=value;
          mutex_unlock(&lock[to]);
       }else{
       printk(KERN_INFO "Invalid input\n");
       }
    }
    else if(message[0]==98&& message[1]==44)//b
    {
      if(from>=0&&to==-1){

         if(account[from]>=value){
          mutex_lock(&lock[from]);
          account[from]-=value;
          mutex_unlock(&lock[from]);
         }
         else{
         printk(KERN_INFO "Source doesn't have enough money\n");
         }
       }else{
       printk(KERN_INFO "Invalid input\n");
       }
    }
    else{
    printk(KERN_INFO "Invalid input\n");
    }
    return len;

}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "EBBChar: Device successfully closed\n");
   return 0;
}

module_init(ebbchar_init);
module_exit(ebbchar_exit);
