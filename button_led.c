#include <linux/interrupt.h>
#include <linux/gpio.h>

int led_state = 0; 
static irqreturn_t button_isr(int irq, void *dev_id, struct pt_regs *regs) 
{ 
	gpio_set_value(149, led_state); 
    gpio_set_value(150, led_state = led_state > 0 ? 0 : 1); 
	return IRQ_HANDLED; 
} 

static struct gpio led_gpios [] = { 
	{149, GPIOF_OUT_INIT_LOW, "USER LED 0" }, 
	{150, GPIOF_OUT_INIT_LOW, "USER LED 1" }, 
}; 

static int button_gpio= 4; /* USER BUTTON */ 
static unsigned int button_irq; 
static spinlock_t g_lock; 

static int isr_test_init(void) 
{ 
	int ret, target_gpio; 

	if (gpio_is_valid(led_gpios[0].gpio) < 0) { 
		printk (KERN_ERR "[CH08] USER LED 0 is not valid.\n") ; 
		goto error; 
	} 

	if (gpio_is_valid(led_gpios[1].gpio) < 0) { 
		printk (KERN_ERR "[CH08] USER LED 1 is not valid.\n"); 
		goto error; 
	} 

	ret = gpio_request_one(button_gpio, GPIOF_IN, 
		"LED status change button"); 
	if (ret < 0) { 
		printk (KERN_ERR "failed to request GPIO %d, error %d\n", 
		button_gpio, ret) ; 
		goto error; 
	} 

	ret = gpio_request_array(led_gpios, ARRAY_SIZE(led_gpios)); 
	if (ret < 0) { 
		printk (KERN_ERR "failed to request GPIO array for LEDs, error %d\n", 
			ret); 
		goto error; 
	} 

	ret = gpio_direction_output(led_gpios[0].gpio, 0); 
	if(ret < 0) { 
		target_gpio = led_gpios[0].gpio; 
		goto error_conf; 
	} 
	ret = gpio_direction_output(led_gpios[1].gpio, 0); 
	if (ret < 0) { 
		target_gpio = led_gpios[1].gpio; 
		goto error_conf; 
	} 
	ret = gpio_direction_input(button_gpio); 
	if (ret < 0) { 
		target_gpio = button_gpio; 
		goto error_conf; 
	} 

	// change button input to IRQ 
	button_irq = gpio_to_irq(button_gpio); 
	if (button_irq < 0) { 
		printk(KERN_ERR "ISR test : Unable to get irq number for \ 
			GPIO %d, error %d\n", button_gpio, button_irq) ; 
		goto error; 
	} 

	/* dev_id should not be NULL if IRQF_SHARED is set */ 
	ret = request_irq(button_irq, button_isr, 
		IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, 
		"ISR-LED interrupt", NULL); 
	if (ret) { 
		printk (KERN_ERR "Unable to claim irq %d; error %d\n", 
		button_irq, ret); 
		return -EBUSY; 
	} 

	spin_lock_init(&g_lock); 
	printk(KERN_ERR "Initializing ISR implementation for LEDs \ 
		completed.\n"); 
	return 0; 

error_conf: 
	printk(KERN_ERR "failed to configure direction for GPIO %d,\ 
		error %d\n", target_gpio, ret); 

error: 
	return -EBUSY; 
} 

static void isr_test_exit(void)
{ 
	gpio_free(button_gpio); 
	gpio_freearray(led_gpios, ARRAY_SIZE(led_gpios)); 
	free_irq(button_irq, NULL); 

	printk(KERN_ERR "Clearing ISR test setting completed.\n"); 
}
