# Herbal-Quality-Analyzer
IoT-based herbal adulteration detection system using ESP32, RGB sensor, MQ135, and DHT11 with ThingSpeak cloud integration and live dashboard.

## Project Overview

This project detects adulteration in herbal products like:

* Tulsi 
* Turmeric 
* Dhaniya 
* Ginger 

using IoT sensors and ESP32.

## Technologies Used

* ESP32
* TCS34725 RGB Sensor
* MQ135 Gas Sensor
* DHT11 Sensor
* ThingSpeak Cloud
* HTML Dashboard

## Working Principle

* RGB sensor detects color composition
* MQ135 detects gas/odor
* DHT11 gives temperature & humidity
* Data is sent to ThingSpeak cloud
* Dashboard visualizes data & detects adulteration


##  Detection Logic

* High G/R ratio → Pure
* Low G/R ratio → Adulterated
* Gas value also used for confirmation


## Live Dashboard

Displays:

* Sensor readings
* Graphs
* Final result (PURE / ADULTERATED)


## Project Structure

* /code → Arduino programs
* /dashboard → HTML dashboard
* /images → screenshots



##  Conclusion

This system helps in detecting adulteration in herbal products using low-cost IoT sensors.


##  Developed By

Pranavi Veeramallu,
Hrithika Gundeti,
Tejasvi Pothanaboina 
