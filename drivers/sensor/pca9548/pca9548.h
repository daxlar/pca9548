#ifndef ZEPHYR_DRIVERS_SENSOR_PCA9548_PCA9548
#define ZEPHYR_DRIVERS_SENSOR_PCA9548_PCA9548

#include <zephyr/drivers/sensor.h>

enum pca9548_channel {
    CHANNEL_ZERO = (1u << 0u),
    CHANNEL_ONE = (1u << 1u),
    CHANNEL_TWO = (1u << 2u),
    CHANNEL_THREE = (1u << 3u),
    CHANNEL_FOUR = (1u << 4u),
    CHANNEL_FIVE = (1u << 5u),
    CHANNEL_SIX = (1u << 6u),
    CHANNEL_SEVEN = (1u << 7u),
    CHANNEL_ALL = 0b11111111,
};

enum pca9548_set_gate_attribute {
    CHANNEL_OPEN,
    CHANNEL_CLOSE,
};

#endif