
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>


MODULE_AUTHOR("Nazar Zhadko <nazar.yurie@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_PARM_DESC(count, "Number of times the hello message should be printed.");
MODULE_VERSION("0.3");

static int count = 1;

module_param(count, uint, 0444);

struct hello_event {
    	struct list_head list;
    	ktime_t timestamp;
};

static LIST_HEAD(event_list);

static int __init hello_init(void)
{
	int i;

	if (count==0 || (count>5 && count<10)) {
		pr_warn("Warning\n");
	}
    BUG_ON(count > 10);

   	for (i = 0; i < count; i++) {
        struct hello_event *new_event = kmalloc(sizeof(struct hello_event), GFP_KERNEL);
		
        if (!new_event) {
   			pr_err("Failed to allocate memory\n");
            return -ENOMEM;
		}

        if (i == 4) {
            kfree(new_event);
            new_event = NULL;
        }
		
        INIT_LIST_HEAD(&new_event->list);
        new_event->timestamp = ktime_get();

        list_add_tail(&new_event->list, &event_list);

        pr_info("Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *event, *tmp;

    list_for_each_entry_safe(event, tmp, &event_list, list) {
        pr_info("Event time: %lld ns\n", ktime_to_ns(event->timestamp));
        list_del(&event->list);
        kfree(event);
    }

    pr_info("Program is finished!\n");
}

module_init(hello_init);
module_exit(hello_exit);