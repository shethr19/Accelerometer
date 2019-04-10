# Accelerometer
Accelerometer Data Acquisition System

This project uses:
 > Accelerometer ADXL 337
 > Esduino Xtream 16-bit processor
 > Matlab 2015
 
The accelerometer transmits analog signal to the Esduino Xtream which converts it to digital values, this process is called Analog to Digital Conversion (ADC). Once the ADC is successful, the new data is then transmitted to Matlab through serial communication interface for futher data mapping with respect to time. This so called data is the angle at which the accelerometer transmits values. 

Overall, the angle is measured in the x-axis with the range of 0-90 degrees.
