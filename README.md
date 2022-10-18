<h1> SEIC-IoT-Proj </h1>
<h2> Description: </h2>
<p> Developed for the class Electronic Systems for the Internet of Things. <br>
    The objective of this project was to create a functioning system with multiple devices that communicated with each order in a typical IoT environment. </p>

<p> Basically, the project is comprised of 1 main system and 2 subsystems, heating and ventilation: </p>
<ul>
  <li>
    The main system is composed of a Raspberry Pi 4 with 4 servers with different purposes. 
  </li> 
  <li>
    The heating subsystem is composed of a temperature sensor (DS18B20) and a 1 kW resistor (100 Ω) controlled by an ESP32 connected to a SSR.
    Low voltage is on the microcontroller side (3.3 VDC), optically isolated from the 'high voltage' side (230 VAC), which is the voltage applied to the resistor.
    A circuit breaker was also used on the AC side for extra protection of the load.
  </li> 
</ul> 



<h2> Project Components Summary </h2>
<p> Project based in multiple hardware and software solutions:</p>
<ul>
    <li> <p> Main System, based on a Raspberry Pi 4 with Raspbian and with the following servers: </p> </li>  
    <ul>
      <li> <p> MQTT Broker - Mosquitto </p> </li>  
      <li> <p> Data bridge - Node-RED </p> </li>  
      <li> <p> Real-time database - InfluxDB </p> </li>  
      <li> <p> Dashboard - Grafana </p> </li>  
    </ul>
    <li> <p> Two subsystems: </p> </li> 
    <ul>
      <li> <p> Heating Subsystem, based on a ESP8266 with a sensor shield. </li>  
      <li> <p> Ventilation Subsystem, based on a WeMos LOLIN32 Lite (ESP32). </p> </li>  
    </ul>
</ul>
