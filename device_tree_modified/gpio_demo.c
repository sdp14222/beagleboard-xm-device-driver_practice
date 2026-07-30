#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/workqueue.h>

static int debounce_ms = 50;
module_param(debounce_ms, int, 0644);
MODULE_PARM_DESC(debounce_ms, "Debounce time in milliseconds");

struct gpio_demo_data {
	struct gpio_desc *led[2];
	struct gpio_desc *button;
	int irq;
	struct delayed_work debounce_work;
};

static void gpio_demo_debounce_work(struct work_struct *work)
{
        struct gpio_demo_data *data = container_of(work, struct gpio_demo_data,
                                                    debounce_work.work);

        gpiod_set_value(data->led[0], !gpiod_get_value(data->led[0]));
        gpiod_set_value(data->led[1], !gpiod_get_value(data->led[1]));

	trace_printk("EFFECTIVE_TOGGLE led0=%d\n", gpiod_get_value(data->led[0]));

        pr_info("gpio_demo: debounced, LEDs toggled\n");
}

static irqreturn_t gpio_demo_isr(int irq, void *dev_id)
{
	struct gpio_demo_data *data = dev_id;

	mod_delayed_work(system_wq, &data->debounce_work,
				msecs_to_jiffies(debounce_ms));
	
	return IRQ_HANDLED;
}

static int gpio_demo_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct gpio_demo_data *data;
	int ret;

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	/* "led-gpios" property : index 0 */
	data->led[0] = devm_gpiod_get_index(dev, "led", 0, GPIOD_OUT_LOW);
	if (IS_ERR(data->led[0]))
		return dev_err_probe(dev, PTR_ERR(data->led[0]), "led0 get failed\n");

	/* "led-gpios" property : index 1 */
	data->led[1] = devm_gpiod_get_index(dev, "led", 1, GPIOD_OUT_LOW);
	if (IS_ERR(data->led[1]))
		return dev_err_probe(dev, PTR_ERR(data->led[1]), "led1 get failed\n");

	/* "button-gpios" property */
	data->button = devm_gpiod_get(dev, "button", GPIOD_IN);
	if (IS_ERR(data->button))
		return dev_err_probe(dev, PTR_ERR(data->button), "button get failed\n");

	/* GPIO -> IRQ mapping */
	data->irq = gpiod_to_irq(data->button);
	if (data->irq < 0)
		return data->irq;

	INIT_DELAYED_WORK(&data->debounce_work, gpio_demo_debounce_work);
	
	ret = devm_request_irq(dev, data->irq, gpio_demo_isr,
					IRQF_TRIGGER_RISING,
					"gpio_demo_btn", data);

	if (ret)
		return dev_err_probe(dev, ret, "irq request failed\n");

	platform_set_drvdata(pdev, data);
	dev_info(dev, "gpio_demo probed successfully, irq=%d\n", data->irq);
	return 0;
}

static int gpio_demo_remove(struct platform_device *pdev)
{
	struct gpio_demo_data *data = platform_get_drvdata(pdev);

	cancel_delayed_work_sync(&data->debounce_work);
	
	gpiod_set_value(data->led[0], 0);
	gpiod_set_value(data->led[1], 0);

	dev_info(&pdev->dev, "gpio_demo removed\n");
	return 0;
}

static const struct of_device_id gpio_demo_of_match[] = {
	{ .compatible = "mycompany,gpio-demo" },
	{ }
};
MODULE_DEVICE_TABLE(of, gpio_demo_of_match);

static struct platform_driver gpio_demo_driver = {
	.probe  = gpio_demo_probe,
	.remove = gpio_demo_remove,
	.driver = {
		.name = "gpio_demo",
		.of_match_table = gpio_demo_of_match,
	},
};
module_platform_driver(gpio_demo_driver);

MODULE_AUTHOR("sdp");
MODULE_DESCRIPTION("Custom GPIO LED + Button demo driver");
MODULE_LICENSE("GPL");

