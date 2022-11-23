# CSC2003 Team A2 Infrared Module
## Table of Centents
* Wheel Encoder
* Barcode Reader

## Wheel Encoder
Our wheel encoder is able to output the left and right wheel RPM, the Current Speed (M/s) and the Total Distance traveled.<br />
The left and right RPM is derived from the left and right counter which is incremented via a interrupt when the sensor detects a notch has passed.

The formula used to calculate the left and right RPM is "RPM = (FREQ x TIMEVAL) / TOTALNOTCHES", which is used alongside a 1 second timer to calculate the RPM every second.<br />
Where FREQ is the left or right counter, TIMEVAL is 60 to get 1 minute based on the 1 sec timer, TOTALNOTCHES is 20.

The current speed is calculated from the average current distance traveled of the two wheel, using the formula "curSpeed = (((leftCounter + rightCounter) / 2) * NOTCHLENGTH) / 100" to get the current speed in meter/s.<br />
Where NOTCHLENGTH is 1.02cm rounded up, 100 is to convert the speed from cm to meter.<br />
The total distance is an aggregate of from the Current Speed every 1 second. <br />
Using this method will allow us to calculate the speed and distance the wheel traveled without the need for extra calculations using other forumlas which increases the load on the MSP.

## Barcode Reader
Our barcode reader is able to read the 3 different lengths of barcode: Big, Mediam, Small.
