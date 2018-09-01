from smbus import SMBus
import time

bus = SMBus(1)

ADDRESS = 1

I2C_LED1 = 1

while True:
    bus.write_i2c_block_data(ADDRESS, I2C_LED1, [80, 0, 0])
    time.sleep(2)
    bus.write_i2c_block_data(ADDRESS, I2C_LED1, [0, 80, 0])
    time.sleep(2)
    bus.write_i2c_block_data(ADDRESS, I2C_LED1, [0, 0, 80])
    time.sleep(2)
