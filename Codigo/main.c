#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "dht.h"
#include "hardware/pio.h"
#include "lcd.h"

#define RELAY_PIN 3
#define SENSOR_PIN 20
#define ANALOG_GPIO 26 
#define ANALOG_CH 0

// DHT usado (cambiar por DHT22 si fuese el caso)
static const dht_model_t DHT_MODEL = DHT11;
// GPIO a usar para los datos
static const uint8_t DATA_PIN = 16;

float humidity, temperature_c;

dht_result_t measure_temp_and_hum(dht_t *dht, float *humidity, float *temperature_c);

int main() {
    stdio_init_all();
    adc_gpio_init(RELAY_PIN);
    adc_select_input(0);
    i2c_init(i2c0, 400 * 1000); 
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    //lcd_init(); //Inica la pantalla LCD
    dht_t dht;
    // Inicializacion de DHT usando el PIO0 para controlar el DATA_PIN

    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, 1);
    lcd_init();
    char str [16];

    dht_result_t result = measure_temp_and_hum(&dht, &humidity, &temperature_c);

    //Borrar si esta mal la siguiente linea
    uint16_t moisture_data = adc_read();
    gpio_init(RELAY_PIN);
    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);
    gpio_set_dir(RELAY_PIN, GPIO_OUT);
    while(1) {
        uint16_t moisture_data = adc_read();
        float humidity_percentage = moisture_data * (-100)/4095 + 100;
        printf("Hum T: %d\n", moisture_data);
        if(moisture_data <= 2049) {
            gpio_put(RELAY_PIN, 1);
        } else if(moisture_data >= 2048) {
            gpio_put(RELAY_PIN, 0);
        }
        sleep_ms(1000);
    }
    return 0;
}

dht_result_t measure_temp_and_hum(dht_t *dht, float *humidity, float *temperature_c) {
     // Inicio comunicacion con sensor
        dht_start_measurement(dht);
        // Esperar a que la comunicacion termine y obtener los datos
        dht_result_t result = dht_finish_measurement_blocking(dht, &humidity, &temperature_c);
        // Verificar el resultado de la operacion
        if (result == DHT_RESULT_OK) {
            // Mostrar temperatura y humedad
            printf("Temp: %2f°C\n", temperature_c);
            printf("Hum: %2f%%\n", humidity);
        } else if (result == DHT_RESULT_TIMEOUT) {
            // Mensaje de error
            puts("El DHT no responde, probablemente este mal el conexionado");
        } else {
            // Error de checksum
            puts("Mal checksum");
        }
        // Demora para no saturar la consola
        sleep_ms(500);
    
    return result;
}