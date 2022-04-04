# Unlockdown

Tired of kernel lockdown? You want to hibernate but your distro won't let you?

There are three options for you:

- Disable Secure Boot in UEFI
- Recompile your own kernel without lockdown
- Use this module

## How to use

This has been tested only on Debian unstable, but should work in Debian-like
distros. If you're on a different distro, you may need to edit the Makefile to
compile and install the module.

```console
$ make dkmsInstall
$ echo 'unlockdown' | sudo tee --append /etc/initramfs-tools/modules
$ sudo update-initramfs -u -k all
```

Note: the module must be loaded in the initrd otherwise the kernel will not be
able to resume from hibernation since it will be back into lockdown.

Reboot and your kernel should be unlocked and hibernation should work!

```console
# dmesg | grep unlockdown
[    8.501198] unlockdown: loading out-of-tree module taints kernel.
[    8.540051] unlockdown: kernel lockdown lifted
# pm-is-supported --hibernate && echo 'Hibernate is supported'
Hibernate is supported
```

## Credits

The initial idea and most of this code come from
[@tux3](https://github.com/tux3), thanks to him!
