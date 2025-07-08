/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
#include "LPS22DF.h"
#include "LSM6DSV16X.h"

//Pines de los sensores
DigitalIn d0_value(D8);

// Pines I2C (ajústalos según tu placa)
#define I2C_SDA PB_9
#define I2C_SCL PB_8

// Instancias de sensores
LPS22DF sensor_presion(I2C_SDA, I2C_SCL);
LSM6DSV16X sensor_acelerometro(I2C_SDA, I2C_SCL, 0x6B); // Dirección I2C forzada a 0x6B

Serial pc(USBTX, USBRX, 115200); // Configuración de comunicación serie con baudrate explícito

int main() {
    pc.printf("\nIniciando sensores...\n");
    int d0_meas;
    
    // Inicializar el sensor de presión/temperatura
    if(sensor_presion.begin() != LPS22DF_OK) {
        pc.printf("Error al inicializar el sensor LPS22DF!\n");
        while(1);
    }
    if(sensor_presion.Enable() != LPS22DF_OK) {
        pc.printf("Error al habilitar el sensor LPS22DF!\n");
        while(1);
    }
    sensor_presion.SetOutputDataRate(25.0f);

    // Inicializar el acelerómetro
    if(sensor_acelerometro.begin() != LSM6DSV16X_OK) {
        pc.printf("Error al inicializar el LSM6DSV16X!\n");
        while(1);
    }
    if(sensor_acelerometro.Enable_X() != LSM6DSV16X_OK) {
        pc.printf("Error al habilitar el acelerómetro LSM6DSV16X!\n");
        while(1);
    }
    sensor_acelerometro.Set_X_ODR(25.0f);
    sensor_acelerometro.Set_X_FS(4); // Configura el rango a ±4g (puedes ajustar este valor según lo requerido)

    float pressure, temperature;
    int32_t acceleration[3];  // Almacenará X, Y, Z de la aceleración

    while(1) {
        
        // Leer presión
        if(sensor_presion.GetPressure(&pressure) == LPS22DF_OK) {
            pc.printf("Presion: %.2f hPa\t", pressure);
        } else {
            pc.printf("Error lectura presion!\t");
        }
        //Leer Gas
        d0_meas = d0_value.read();
        printf("  Digital Value: %d -> %s\n", d0_meas, d0_meas ? "No gas detected" : "Gas detected!");
        
        // Leer temperatura
        if(sensor_presion.GetTemperature(&temperature) == LPS22DF_OK) {
            pc.printf("Temperatura: %.2f C\t", temperature);
        } else {
            pc.printf("Error lectura temperatura!\t");
        }

        // Leer aceleración (los valores se retornan como un arreglo de int32_t)
        if(sensor_acelerometro.Get_X_Axes(acceleration) == LSM6DSV16X_OK) {
            pc.printf("Accel [mg]: X=%d, Y=%d, Z=%d\n", acceleration[0], acceleration[1], acceleration[2]);
        } else {
            pc.printf("Error lectura aceleración!\n");
        }

        ThisThread::sleep_for(1000); // Esperar 1 segundo
    }
}
