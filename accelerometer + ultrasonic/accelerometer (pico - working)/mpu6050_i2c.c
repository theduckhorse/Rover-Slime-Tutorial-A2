/********************************************************************************/
/*  Accelerometer codes for A2:                                                 */
/*  By: CHEO CHEANG MING                                                        */
/*      AHMAD FAIRUZI BIN KADIR SAHIB                                           */
/*      POH KAI BOON                                                            */
/*                                                                              */
/*                                                                              */
/*  Credits to:                                                                 */
/*  https://github.com/raspberrypi/pico-examples/tree/master/i2c/mpu6050_i2c    */
/*  for reference on MPU6050 sensor                                             */
/*                                                                              */
/********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// Address of MPU6050
static int SLAVE_ADDRESS = 0x68;

#ifdef i2c_default
static void mpu6050_reset()
{
    // Reset MPU6050's. First byte register, second byte data
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, SLAVE_ADDRESS, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3])
{
    // Send the slave the register we want to read from first. Register address wil auto-increment after each read

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    // We are reading 6 bytes because each axis (X, Y, Z) has 2 registers one for High Byte, one for Low Byte (2 x 3 = 6)
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, SLAVE_ADDRESS, &val, 1, true);
    i2c_read_blocking(i2c_default, SLAVE_ADDRESS, buffer, 6, false);

    // Combining the High and Low bytes of each axis and storing it into accel[] array
    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]) / 16384.0;
    }

    // Start reading gyroscope registers from register 0x43 for 6 bytes
    val = 0x43;
    i2c_write_blocking(i2c_default, SLAVE_ADDRESS, &val, 1, true);
    i2c_read_blocking(i2c_default, SLAVE_ADDRESS, buffer, 6, false);

    // Combining the High and Low bytes of each axis and storing it into gyro[] array
    for (int i = 0; i < 3; i++)
    {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]) / 131.0;
        ;
    }
}
#endif

// static void calculate rollPitchYaw(){
//         float accAngleX = (atan(acceleration[1] / sqrt(pow(acceleration[0], 2) + pow(acceleration[2], 2))) * 180 / PI) - 0.58;
//         float accAngleY = (atan(-1 * acceleration[0] / sqrt(pow(acceleration[0], 2) + pow(acceleration[2], 2))) * 180 / PI) + 1.58;
//         float gyroX = gyro[0] + 0.56;
//         float gyroY = gyro[1] - 2;
//         float gyroZ = gyro[2] + 0.79;

//         float roll = 0.96 * gyroX + 0.04 * accAngleX;
//         float pitch = 0.96 * gyroY + 0.04 * accAngleY;

//         printf("Roll = %d\n", roll);
//         printf("Pitch = %d\n", pitch);
//         printf("Yaw = %d\n\n", gyroZ);
// }

int main()
{
    // Enable UART so we can print status output
    stdio_init_all();

#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    printf("Hello, MPU6050! Reading raw data from registers...\n");

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();
    int16_t acceleration[3], gyro[3];

    while (1)
    {
        mpu6050_read_raw(acceleration, gyro);

        printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Gyro. X = %d, Y = %d, Z = %d\n\n", gyro[0], gyro[1], gyro[2]);
        sleep_ms(1000);
    }

#endif
    return 0;
}