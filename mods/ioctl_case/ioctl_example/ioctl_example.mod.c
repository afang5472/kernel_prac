#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa37e06fb, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x524b578b, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x30142270, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xa88d2147, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x8b2afef5, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x35c6f387, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x7b847751, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x1e6494df, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x6220305, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0xb8e7ce2c, __VMLINUX_SYMBOL_STR(__put_user_8) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x9f6241b7, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xf2a9b766, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8C4CCF2F7A3E7AF2E4FEAF4");
