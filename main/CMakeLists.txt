idf_component_register(SRCS "bme280.c"
                    INCLUDE_DIRS "."
                    REQUIRES BME280 i2cdev driver)

