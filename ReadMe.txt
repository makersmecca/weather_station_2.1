ESP8266 based Weather Station with NTP Clock and Open Weather

Updated the code from the original Weather Station project (1.0)

Hardware Changes:
-> Replaced 16x2 LCD with 20x4 LCD
-> Replaced momentary pushed buttons with compact push buttons
-> Removed Smoke Sensor as it wasn't required for my use case

Software Updates:
-> Added Open Weather API to collect and display weather related data
---> Weather details <---
	* Temperature
	* Humidity
	* Atmospheric Pressure
	* Wind Speed
	* Weather condition
	* Minimum/Maximum Teperature
	* Feels like(temperature)
-> Measures and displays room temperature too
-> Added Wifi details option (SSID, Local IP)


** temperatures are in °C || pressure is in hPa || Wind Speed is in m/s **

