#define DT_DRV_COMPAT ti_pca9548

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdlib.h>
#include <math.h>
#include <zephyr/logging/log.h>

#include "pca9548.h"

LOG_MODULE_REGISTER(PCA9548, CONFIG_SENSOR_LOG_LEVEL);

struct pca9548_data {
    uint8_t channel_states;
};

struct pca9548_dev_config {
    struct i2c_dt_spec i2c;
};

static int pca9548_sample_fetch(const struct device *dev, 
                                enum sensor_channel chan) {
	
    return 0;
}

static int pca9548_channel_get(const struct device *dev, 
                               enum sensor_channel chan, 
                               struct sensor_value *val) {


	if(chan < CHANNEL_ZERO || chan > CHANNEL_ALL) {
		LOG_ERR("invalid channel number to get \r\n");
		return -EINVAL;
	}	

	const struct pca9548_dev_config *config = dev->config;

	uint8_t channel_state; 
	uint8_t i2c_operation_error = i2c_read_dt(&config->i2c, &channel_state, 1);
	if(i2c_operation_error) {
		LOG_ERR("failure retrieving pca9548 channel state at channel_get\r\n");
		return -ENODEV;
	}

	val->val1 = (channel_state && chan);

    return 0;
}

static int pca9548_attr_set(const struct device *dev, 
                            enum sensor_channel chan, 
                            enum sensor_attribute attr, 
                            const struct sensor_value *val) {

	(void*)val;

	if(chan < CHANNEL_ZERO || chan > CHANNEL_ALL) {
		LOG_ERR("invalid channel number to get \r\n");
		return -EINVAL;
	}	

	if(attr != CHANNEL_CLOSE && attr != CHANNEL_OPEN) {
		LOG_ERR("invalid channel operation \r\n");
		return -EINVAL;
	}

    const struct pca9548_dev_config *config = dev->config;
	
	/// add functionality for closing specific channels

	printk("channel to open: %d \r\n", chan);

	uint8_t channels_to_set = chan;
	uint8_t i2c_operation_error = i2c_write_dt(&config->i2c, &channels_to_set, 1);
	if(i2c_operation_error) {
		LOG_ERR("failure writing pca9548 channel state at attr_set \r\n");
		return -ENODEV;
	}
	
	uint8_t channels_to_read;
	i2c_operation_error = i2c_read_dt(&config->i2c, &channels_to_read, 1);
	if(i2c_operation_error) {
		LOG_ERR("failure retrieving pca9548 channel state at attr_set \r\n");
		return -ENODEV;
	}

	printk("channels open: %d \r\n", channels_to_read);

	if((channels_to_set != chan) || (channels_to_read != channels_to_set)) {
		LOG_ERR("couldn't set the channels at attr_set \r\n");
		return -EINVAL;
	}

    return 0;
}

int pca9548_set_initial_registers(const struct device *dev) {

	const struct pca9548_dev_config *config = dev->config;

	uint8_t no_channels_open_write = 0;
	uint8_t no_channels_open_read = 0;

	uint8_t i2c_operation_error = i2c_write_dt(&config->i2c, &no_channels_open_write, 1);
	if(i2c_operation_error) {
		LOG_ERR("failure with i2c bus during pca9548 initialization stage 1 \r\n");
		return -ENODEV;
	}

	i2c_operation_error = i2c_read_dt(&config->i2c, &no_channels_open_read, 1);
	if(i2c_operation_error) {
		LOG_ERR("failure with i2c bus during pca9548 initialization stage 2 \r\n");
		return -ENODEV;
	}

	printk("channel_write: %d, channel_read: %d \r\n", no_channels_open_write, no_channels_open_read);

	if(no_channels_open_read != no_channels_open_write) {
		LOG_ERR("failure with writing to pca9548 initialization stage 3 \r\n");
		return -ENODEV;
	}

	return 0;
}

int pca9548_init(const struct device *dev) {

    const struct pca9548_dev_config *config = dev->config;
    
    if (!device_is_ready(config->i2c.bus)) {
        LOG_ERR("failure with i2c bus \r\n");
		return -ENODEV;
	}

    /// initialize sensor here

    if(pca9548_set_initial_registers(dev) < 0) {
        LOG_ERR("failure initializing pca9548 \r\n");
        return -EINVAL;
    }

	return 0;
}

static const struct sensor_driver_api pca9548_api_funcs = {
	.sample_fetch = pca9548_sample_fetch,
	.channel_get = pca9548_channel_get,
	.attr_set = pca9548_attr_set,
};

#define PCA9548_DEVICE_INIT(inst)					\
	SENSOR_DEVICE_DT_INST_DEFINE(inst,				\
			      pca9548_init,				        \
			      NULL,					            \
			      &pca9548_data_##inst,			    \
			      &pca9548_config_##inst,			\
			      POST_KERNEL,				        \
			      CONFIG_SENSOR_INIT_PRIORITY,		\
			      &pca9548_api_funcs);

#define PCA9548_CONFIG(inst)								\
    {                                                       \
		.i2c = I2C_DT_SPEC_INST_GET(inst),					\
    }                                                       \

#define PCA9548_DEFINE(inst)                                        \
    static struct pca9548_data pca9548_data_##inst;                 \
    static const struct pca9548_dev_config pca9548_config_##inst =	\
        PCA9548_CONFIG(inst);				                        \
    PCA9548_DEVICE_INIT(inst)

DT_INST_FOREACH_STATUS_OKAY(PCA9548_DEFINE)
