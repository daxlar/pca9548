
zephyr_library()
zephyr_library_sources_ifdef(CONFIG_PCA9548 pca9548.c)