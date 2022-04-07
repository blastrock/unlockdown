#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Philippe Daouadi");
MODULE_DESCRIPTION("Remove kernel lockdown after boot");
MODULE_VERSION("0.01");

unsigned long (*kla)(const char* name);
uint8_t* kernel_locked_down;
uint8_t old_value;

static int resolve_kla(void)
{
  struct kprobe kla_kp = {.symbol_name = "kallsyms_lookup_name"};

  int ret = register_kprobe(&kla_kp);
  if (ret < 0)
  {
    pr_err("unlockdown: register_kprobe failed (%pe)\n", ERR_PTR(ret));
    return ret;
  }
  pr_devel("unlockdown: resolved kallsyms_lookup_name\n");
  kla = (void*)kla_kp.addr;
  unregister_kprobe(&kla_kp);
  return 0;
}

static int __init mod_init(void)
{
  int err = resolve_kla();
  if (err < 0)
    return err;
  kernel_locked_down = (uint8_t*)kla("kernel_locked_down");
  if (!kernel_locked_down)
  {
    printk(KERN_ERR "unlockdown: failed to find kernel_locked_down symbol\n");
    return -ENOENT;
  }
  old_value = *kernel_locked_down;
  *kernel_locked_down = 0;
  printk(KERN_INFO "unlockdown: kernel lockdown lifted\n");
  return 0;
}

static void __exit mod_exit(void)
{
  *kernel_locked_down = old_value;
  printk(KERN_INFO "unlockdown: kernel lockdown restored\n");
}

module_init(mod_init);
module_exit(mod_exit);
