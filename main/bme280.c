#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "bme280.h"

#define I2C_MASTER_SCL_IO          22        /*!< GPIO cho chân SCL */
#define I2C_MASTER_SDA_IO          21        /*!< GPIO cho chân SDA */
#define I2C_MASTER_NUM             I2C_NUM_0 /*!< I2C port */
#define I2C_MASTER_FREQ_HZ         100000    /*!< Tốc độ I2C (100kHz) */

#define BME280_I2C_ADDRESS         0x76      /*!< Địa chỉ I2C của BME280 */

//Thiết lập giao tiếp I2C
esp_err_t i2c_master_init(void) {
    int i2c_master_port = I2C_MASTER_NUM;
    
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        ESP_LOGE("I2C", "Ket noi I2C that bai");
        return err;
    }

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

//Kiểm tra kết nối BME280
esp_err_t bme280_check_connection(void) {
    bme280 bme280_device;
    bmp280_params_t bme280_params;
    
    // Khởi tạo BME280
    esp_err_t ret = bme280_init(&bme280_device, &bme280_params, BME280_I2C_ADDRESS, I2C_MASTER_NUM, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    if (ret != ESP_OK) {
        ESP_LOGE("BME280", "Không thể kết nối với BME280!");
        return ret;
    }
    ESP_LOGI("BME280", "Kết nối với BME280 thành công!");
    return ESP_OK;
}

//Đọc dữ liệu nhiệt độ và độ ẩm từ BME280
void read_bme280_data(void) {
    bme280 bme280_device;
    float temperature, pressure, humidity;

    esp_err_t ret = bme280_readSensorData(&bme280_device, &temperature, &pressure, &humidity);
    if (ret == ESP_OK) {
        ESP_LOGI("BME280", "Nhiet do: %.2f °C, Ap suat: %.2f hPa, Do am: %.2f%%", 
                 temperature, pressure / 100.0, humidity);
    } else {
        ESP_LOGE("BME280", "Khong the lay du lieu tu BME280");
    }
}
