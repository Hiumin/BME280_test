#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "bme280.h"
//test123
#define I2C_MASTER_SCL_IO          22        /*!< GPIO cho chân SCL */
#define I2C_MASTER_SDA_IO          21        /*!< GPIO cho chân SDA */
#define I2C_MASTER_NUM             I2C_NUM_0 /*!< I2C port */
#define I2C_MASTER_FREQ_HZ         100000    /*!< Tốc độ I2C (100kHz) */
#define I2C_MASTER_TX_BUF_DISABLE  0         /*!< Không sử dụng buffer TX */
#define I2C_MASTER_RX_BUF_DISABLE  0         /*!< Không sử dụng buffer RX */
#define I2C_MASTER_TIMEOUT_MS      1000      /*!< Timeout cho I2C */

#define BME280_I2C_ADDRESS         0x76      /*!< Địa chỉ I2C của BME280 */

static const char *TAG = "BME280_TEST";

/**
 * @brief Thiết lập giao tiếp I2C
 */
static esp_err_t i2c_master_init(void) {
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
        ESP_LOGE(TAG, "Cấu hình I2C thất bại");
        return err;
    }

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief Kiểm tra kết nối BME280
 */
static esp_err_t bme280_check_connection() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "BME280 đã kết nối thành công!");
    } else {
        ESP_LOGE(TAG, "Không tìm thấy BME280! Vui lòng kiểm tra dây kết nối.");
    }

    return ret;
}

/**
 * @brief Hàm đọc thanh ghi từ BME280
 */
static esp_err_t bme280_read_register(uint8_t reg_addr, uint8_t *data, uint16_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Đọc dữ liệu nhiệt độ và độ ẩm từ BME280
 */
static void read_bme280_data(void) {
    uint8_t data[8];
    int32_t raw_temp, raw_hum;
    float temperature, humidity;

    while (1) {
        // Đọc dữ liệu từ thanh ghi nhiệt độ và độ ẩm
        if (bme280_read_register(0xF7, data, 8) == ESP_OK) {
            raw_temp = (int32_t)((((uint32_t)data[3] << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5]) >> 4);
            raw_hum = (int32_t)(((uint32_t)data[6] << 8) | (uint32_t)data[7]);

            // Chuyển đổi dữ liệu thô sang nhiệt độ và độ ẩm thực tế
            temperature = raw_temp / 5120.0;
            humidity = raw_hum / 1024.0;

            ESP_LOGI(TAG, "Nhiệt độ: %.2f °C, Độ ẩm: %.2f%%", temperature, humidity);
        } else {
            ESP_LOGE(TAG, "Không thể đọc dữ liệu từ BME280");
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Chờ 2 giây
    }
}

/**
 * @brief Chương trình chính
 */
void app_main(void) {
    ESP_LOGI(TAG, "Bắt đầu kiểm tra BME280");

    // Khởi tạo I2C
    if (i2c_master_init() == ESP_OK) {
        ESP_LOGI(TAG, "I2C đã được khởi tạo thành công");
    } else {
        ESP_LOGE(TAG, "Lỗi khi khởi tạo I2C");
        return;
    }

    // Kiểm tra kết nối với BME280
    bme280_check_connection();

    //Đọc dữ liệu từ BME280
    read_bme280_data();
}
