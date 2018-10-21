from smbus import SMBus
import time

bus = SMBus(1)

ADDRESS = 1

I2C_VALUE = 0
I2C_LED1 = 1
I2C_LED2 = 2
I2C_MOTORS = 3

while True:
    bus.write_i2c_block_data(ADDRESS, I2C_LED1, [22, 10, 2])
    time.sleep(2)
    bus.write_i2c_block_data(ADDRESS, I2C_LED1, [44, 20, 4])
    time.sleep(2)
    #bus.write_i2c_block_data(ADDRESS, I2C_MOTORS, [1, 60, -1, 70])
    #time.sleep(2)
    bus.write_i2c_block_data(ADDRESS, I2C_LED2, [60, 0, 0])
    time.sleep(2)
    bus.write_i2c_block_data(ADDRESS, I2C_LED2, [0, 60, 0])
    time.sleep(2)
    bus.write_i2c_block_data(ADDRESS, I2C_LED2, [0, 0, 60])
    time.sleep(2)
    #bus.write_i2c_block_data(ADDRESS, I2C_MOTORS, [1, 0, 1, 0])
    data = bus.read_i2c_block_data(ADDRESS, I2C_VALUE)
    print("data =", " ".join(map(hex, data)))