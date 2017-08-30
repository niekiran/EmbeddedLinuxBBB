/*============================================================================
 Name        : mpu6050_rawread.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     :
 Copyright   : Your copyright notice
 Description : This Application prints Accelerometer and gyroscope raw and G values using MPU6050 sensor
 TODOs for the students :
 ============================================================================*/


/*================================================
BBB_expansion_header_P9_pins     MPU6050 pins
===================================================
P9_19                              SCL
P9_20                              SDA
P9_3                               VCC 3.3v
P9_1                               GND
==================================================== */

/*
 * Datasheet refs
 * 1. MPU-6000 and MPU-6050 Product Specification Revision 3.4
 * 2. MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2
 */


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

/*MPU6050 register addresses */

#define MPU6050_REG_POWER               0x6B
#define MPU6050_REG_ACCEL_CONFIG        0x1C
#define MPU6050_REG_GYRO_CONFIG         0x1B

/*These are the addresses of mpu6050 from which you will fetch accelerometer x,y,z high and low values */
#define MPU6050_REG_ACC_X_HIGH          0x3B
#define MPU6050_REG_ACC_X_LOW           0x3C
#define MPU6050_REG_ACC_Y_HIGH          0x3D
#define MPU6050_REG_ACC_Y_LOW           0x3E
#define MPU6050_REG_ACC_Z_HIGH          0x3F
#define MPU6050_REG_ACC_Z_LOW           0x40

/*These are the addresses of mpu6050 from which you will fetch gyro x,y,z high and low values */
#define MPU6050_REG_GYRO_X_HIGH          0x43
#define MPU6050_REG_GYRO_X_LOW           0x44
#define MPU6050_REG_GYRO_Y_HIGH          0x45
#define MPU6050_REG_GYRO_Y_LOW           0x46
#define MPU6050_REG_GYRO_Z_HIGH          0x47
#define MPU6050_REG_GYRO_Z_LOW           0x48

/*
 * Different full scale ranges for acc and gyro
 * refer table 6.2 and 6.3 in the document MPU-6000 and MPU-6050 Product Specification Revision 3.4
 *
 */
#define ACC_FS_SENSITIVITY_0					16384
#define ACC_FS_SENSITIVITY_1		            8192
#define ACC_FS_SENSITIVITY_2		            4096
#define ACC_FS_SENSITIVITY_3		            2048

#define GYR_FS_SENSITIVITY_0					 131
#define GYR_FS_SENSITIVITY_1					 65.5
#define GYR_FS_SENSITIVITY_2					 32.8
#define GYR_FS_SENSITIVITY_3				 	 16.4


/* This is the I2C slave address of mpu6050 sensor */
#define MPU6050_SLAVE_ADDR 0x68

#define MAX_VALUE 50

/* This is the linux OS device file for hte I2C3 controller of the SOC */
#define I2C_DEVICE_FILE   "/dev/i2c-2"

int fd;

/*write a 8bit "data" to the sensor at the address indicated by "addr" */
int mpu6050_write(uint8_t addr, uint8_t data)
{
  int ret;
  char buf[2];
  buf[0]=addr;
  buf[1]=data;
  ret = write(fd,buf,2);
  if (ret <= 0)
  {
      perror("write failed\n");
      return -1;
  }
  return 0;
}

/*read "len" many bytes from "addr" of the sensor in to the adresss indicated by "pBuffer" */
int mpu6050_read(uint8_t base_addr, char *pBuffer,uint32_t len)
{
  int ret;
  char buf[2];
  buf[0]=base_addr;
  ret = write(fd,buf,1);
  if (ret <= 0)
  {
      perror("write address failed\n");
      return -1;
  }

  ret = read(fd,pBuffer,len);
  if(ret <= 0)
  {
      perror("read failed\n");
  }
  return 0;
}


/* by default mpu6050 will in sleep mode, so disable its sleep mode and also configure 
 * the full scale ranges for gyro and acc
 */
void mpu6050_init()
{
    // 1. disable sleep mode
    mpu6050_write(MPU6050_REG_POWER, 0x00);
    usleep(500);

    // Adjust full scale values for gyro and acc
    mpu6050_write(MPU6050_REG_ACCEL_CONFIG, 0x00);
    usleep(500);
    mpu6050_write(MPU6050_REG_GYRO_CONFIG, 0x00);
    usleep(500);
}

/* read accelerometer values of x,y,z in to the buffer "pBuffer" */
void mpu6050_read_acc(short int *pBuffer)
{
    //each axis value is of 2byte, so we need a buffer of 6bytes. 
    char acc_buffer[6];
    
    //start reading from the base address of accelerometer values i.e MPU6050_REG_ACC_X_HIGH
    mpu6050_read(MPU6050_REG_ACC_X_HIGH,acc_buffer,6);
    
    /* pBuffer[0]= acc x axis value , pBuffer[1]= acc y axis value , pBuffer[2]= acc z axis value  */
    pBuffer[0] = (int) ( (acc_buffer[0] << 8) |  acc_buffer[1] );
    pBuffer[1] = (int) ( (acc_buffer[2] << 8) |  acc_buffer[3] );
    pBuffer[2] = (int) ( (acc_buffer[4] << 8) |  acc_buffer[5] );

}

/* read gyro values of x,y,z in to the buffer "pBuffer" */
void mpu6050_read_gyro(short *pBuffer)
{
    char gyro_buffer[6];
    
    //start reading from the base address of gyro values i.e MPU6050_REG_GYRO_X_HIGH
    mpu6050_read(MPU6050_REG_GYRO_X_HIGH,gyro_buffer,6);

    pBuffer[0] =  ( (gyro_buffer[0] << 8) +  gyro_buffer[1] );
    pBuffer[1] =  ( (gyro_buffer[2] << 8) +  gyro_buffer[3] );
    pBuffer[2] =  ( (gyro_buffer[4] << 8) +  gyro_buffer[5] );

}

int main(void)
{

     short acc_value[3],gyro_value[3];
     double accx,accy,accz,gyrox,gyroy,gyroz;

     /*first lets open the I2C device file */
    if ((fd = open(I2C_DEVICE_FILE,O_RDWR)) < 0) {
        perror("Failed to open I2C device file.\n");
        return -1;
    }

    /*set the I2C slave address using ioctl I2C_SLAVE command */
    if (ioctl(fd,I2C_SLAVE,MPU6050_SLAVE_ADDR) < 0) {
            perror("Failed to set I2C slave address.\n");
            close(fd);
            return -1;
    }

    mpu6050_init();


    while(1)
    {
        mpu6050_read_acc(acc_value);
        mpu6050_read_gyro(gyro_value);

        /*Convert acc raw values in to 'g' values*/
        accx = (double) acc_value[0]/ACC_FS_SENSITIVITY_0;
        accy = (double) acc_value[1]/ACC_FS_SENSITIVITY_0;
        accz = (double) acc_value[2]/ACC_FS_SENSITIVITY_0;

        /* Convert gyro raw values in to  "°/s" (deg/seconds) */
        gyrox = (double) gyro_value[0]/GYR_FS_SENSITIVITY_0;
        gyroy = (double) gyro_value[1]/GYR_FS_SENSITIVITY_0;
        gyroz = (double) gyro_value[2]/GYR_FS_SENSITIVITY_0;

#if 0
        /* print just the raw values read */
       printf("Acc(raw)=> X:%d Y:%d Z:%d gyro(raw)=> X:%d Y:%d Z:%d \n", \
                   acc_value[0],acc_value[1],acc_value[2],gyro_value[0],gyro_value[1],gyro_value[2]);
       
       /* print the 'g' and '°/s' values */
       printf("Acc(g)=> X:%.2f Y:%.2f Z:%.2f gyro(dps)=> X:%.2f Y:%.2f Z:%.2f \n", \
               accx,accy,accz,gyrox,gyroy,gyroz);
#endif

#if 1
       printf("%0.2f	%0.2f	%0.2f\n",accx,accy,accz);
#endif
      
      /*wait for 250000 micro seconds, thats 250ms before going for another round */
       usleep(50 * 1000);
    }

}


