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
    The heating subsystem is composed of a digital temperature sensor (DS18B20) and a 1 kW resistor (100 Ω) controlled by an ESP32 connected to a SSR.
    Low voltage is on the microcontroller side (3.3 VDC), optically isolated from the 'high voltage' side (230 VAC), which is the voltage applied to the resistor.
    A circuit breaker was also used on the AC side for extra protection of the load.
  </li> 
  <li>
    The ventilation subsystem is composed of an analog temperature sensor (LM35), a digital environmental sensor based on a BME680 IC, a 230 VAC fan controlled by an 
    ESP8266, also connected to a SSR. Low voltage is on the microcontroller side (3.3 VDC), optically isolated from the 'high voltage' side (230 VAC), which is the         voltage applied to the fan. A circuit breaker was also used on the AC side for extra protection of the load and a snubber circuit for the SSR high side protection     against high reverse voltage provoked by the fan's coils.
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
      <li> <p> Heating Subsystem, based on a Lolin NodeMCU V3 with a sensor shield. </p> 
      <p align="center"> 
        <img width="380" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/heat_subsys_1.png" alt="Heat Subsys 1"/>
        <img width="300" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/heat_subsys_2.png" alt="Heat Subsys 2"/>
      </p>
      </li>  
      <li> <p> Ventilation Subsystem, based on a WeMos LOLIN32 Lite (ESP32). </p> 
      <p align="center"> 
        <img width="500" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/venti_subsys_1.png" alt="Venti Subsys 1"/>
        <img width="500" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/venti_subsys_1.png" alt="Venti Subsys 2"/>
      </p>
      </li>  
    </ul>
</ul>
