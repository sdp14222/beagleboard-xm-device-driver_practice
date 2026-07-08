// button_led.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>

#define LED0_GPIO    118 /* beagleboard::usr0 */
#define LED1_GPIO    117 /* beagleboard::usr1 */
#define BUTTON_GPIO  164 /* USER button */

static int led_state = 0;
static unsigned long last_jiffies = 0;

static struct gpio led_gpios[] = {
    { LED0_GPIO, GPIOF_OUT_INIT_LOW, "USER LED 0" },
    { LED1_GPIO, GPIOF_OUT_INIT_LOW, "USER LED 1" },
};

static unsigned int button_irq;
static spinlock_t g_lock;

static irqreturn_t button_isr(int irq, void *dev_id)
{
    unsigned long flags;

    /* 아주 단순한 소프트웨어 디바운스: 50ms 이내 재진입 무시 */
    if (time_before(jiffies, last_jiffies + msecs_to_jiffies(50)))
        return IRQ_HANDLED;
    last_jiffies = jiffies;

    spin_lock_irqsave(&g_lock, flags);
    led_state = !led_state;
    spin_unlock_irqrestore(&g_lock, flags);

    gpio_set_value(LED1_GPIO, led_state);
    gpio_set_value(LED0_GPIO, !led_state);

    return IRQ_HANDLED;
}

static int __init button_led_init(void)
{
    int ret;

    if (!gpio_is_valid(LED0_GPIO)) {
        printk(KERN_ERR "button_led: USER LED 0 (GPIO %d) is not valid.\n", LED0_GPIO);
        return -EINVAL;
    }
    if (!gpio_is_valid(LED1_GPIO)) {
        printk(KERN_ERR "button_led: USER LED 1 (GPIO %d) is not valid.\n", LED1_GPIO);
        return -EINVAL;
    }
    if (!gpio_is_valid(BUTTON_GPIO)) {
        printk(KERN_ERR "button_led: USER BUTTON (GPIO %d) is not valid.\n", BUTTON_GPIO);
        return -EINVAL;
    }

    spin_lock_init(&g_lock);

    ret = gpio_request_array(led_gpios, ARRAY_SIZE(led_gpios));
    if (ret < 0) {
        printk(KERN_ERR "button_led: failed to request LED GPIO array, error %d\n", ret);
        return ret;
    }

    ret = gpio_request_one(BUTTON_GPIO, GPIOF_IN, "LED status change button");
    if (ret < 0) {
        printk(KERN_ERR "button_led: failed to request GPIO %d, error %d\n", BUTTON_GPIO, ret);
        goto err_free_leds;
    }

    button_irq = gpio_to_irq(BUTTON_GPIO);
    if (button_irq < 0) {
        printk(KERN_ERR "button_led: unable to get irq number for GPIO %d\n", BUTTON_GPIO);
        ret = button_irq;
        goto err_free_button;
    }

    ret = request_irq(button_irq, button_isr,
                       IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                       "button_led_irq", NULL);
    if (ret) {
        printk(KERN_ERR "button_led: unable to claim irq %d, error %d\n", button_irq, ret);
        goto err_free_button;
    }

    printk(KERN_INFO "button_led: module loaded, LED0=%d LED1=%d BUTTON=%d IRQ=%d\n",
           LED0_GPIO, LED1_GPIO, BUTTON_GPIO, button_irq);
    return 0;

err_free_button:
    gpio_free(BUTTON_GPIO);
err_free_leds:
    gpio_free_array(led_gpios, ARRAY_SIZE(led_gpios));
    return ret;
}

static void __exit button_led_exit(void)
{
    free_irq(button_irq, NULL);
    gpio_free(BUTTON_GPIO);
    gpio_free_array(led_gpios, ARRAY_SIZE(led_gpios));
    printk(KERN_INFO "button_led: module unloaded\n");
}

module_init(button_led_init);
module_exit(button_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sdp");
MODULE_DESCRIPTION("Interrupt-driven button/LED GPIO driver for BeagleBoard-xM");
