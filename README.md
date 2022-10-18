<h1> SEIC-IoT-Proj </h1>
<p> This project was solely developed by myself, with components I had on the shelf. The model of the project, the main system and subsystems was my own call/choice.</p>
<h2> Description: </h2>
<p> Developed for the class Electronic Systems for the Internet of Things. <br> <br>
    The objective of this project was to create a functioning system with multiple devices that communicated with each order in a typical IoT environment. </p>

<p> Basically, the project is comprised of 1 main system and 2 subsystems, heating and ventilation: </p>
<ul>
  <li>
    The main system is composed of a Raspberry Pi 4 (connected to a router phisically with an ethernet cable), with 4 servers with different purposes. 
     <ul>
      <li> <p> MQTT Broker Mosquitto - Allows the arrival of messages into the many MQTT existent MQTT topics; </p> </li>  
      <li> <p> Data bridge Node-RED  - Allows the easy connection between the MQTT broker and the storage in the database; </p> </li>  
      <li> <p> Real-time database InfluxDB - Allows the storage of all the incoming data in the respective fields inside a timetable, through database storage queries;         </p> </li>  
      <li> <p> Dashboard Grafana - Allows the querying of the database in order to pull the data and display it in a graphical, more appealing manner.</p> </li>  
     </ul>
  </li> 
  <li>
    The heating subsystem is composed of a digital temperature sensor (DS18B20) and a 1 kW resistor (100 Ω) controlled by an ESP32 connected to a SSR.
    Low voltage is on the microcontroller side (3.3 VDC), optically isolated from the 'high voltage' side (230 VAC), which is the voltage applied to the resistor.
    A circuit breaker was also used on the AC side for extra protection of the load. The ESP32 was connected to the same network as the Raspberry Pi, but wirelessly, 
    through WiFi.
  </li> 
  <li>
    The ventilation subsystem is composed of an analog temperature sensor (LM35), a digital environmental sensor based on a BME680 IC, a 230 VAC fan controlled by an 
    ESP8266, also connected to a SSR. Low voltage is on the microcontroller side (3.3 VDC), optically isolated from the 'high voltage' side (230 VAC), which is the         voltage applied to the fan. A circuit breaker was also used on the AC side for extra protection of the load and a snubber circuit for the SSR high side protection     against high reverse voltage provoked by the fan's coils. The ESP8266 was connected to the same network as the Raspberry Pi, but wirelessly, through WiFi.
  </li> 
</ul> 

<p> A Telegram bot within a private group was created in order to receive commands and parametrize both subsystems wirelessly (through the same WiFi network), if desired. The temperature setpoint in both subsystems was configurable within a certain range as well as the upper and/or lower hysteresis for the ventilation subsystem. </p>


<h2> Project Components Summary </h2>
<p> Project based in multiple hardware and software solutions:</p>
<ul>
    <li> <p> Main System, based on a Raspberry Pi 4 with Raspbian and with the following servers: </p> </li>  
    <ul>
      <li> <p> MQTT Broker - Mosquitto </p> </li>  
      <li> <p> Data bridge - Node-RED </p> </li>  
      <li> <p> Real-time database - InfluxDB </p> </li>  
      <li> <p> Dashboard - Grafana </p> </li>  
      <p align="center"> 
        <img width="371" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/main_sys_1.png" alt="Main Sys 1"/>
      </p>
    </ul>
    <li> <p> Two subsystems: </p> </li> 
    <ul>
      <li> <p> Heating Subsystem, based on a Lolin NodeMCU V3 with a sensor shield. </p> 
      <p align="center"> 
        <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/heat_subsys_1.png" alt="Heat Subsys 1"/>
        <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/heat_subsys_2.png" alt="Heat Subsys 2"/>
      </p>
      <p align="center"> 
        <img width="600" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/diagram_heat_subsys.png" alt="Diagram Heat Subsys"/>
      </p>
      </li>  
      <li> <p> Ventilation Subsystem, based on a WeMos LOLIN32 Lite (ESP32). </p> 
      <p align="center"> 
        <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/venti_subsys_1.png" alt="Venti Subsys 1"/>
        <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/venti_subsys_2.png" alt="Venti Subsys 2"/>
      </p>
      <p align="center"> 
        <img width="700" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/diagram_venti_subsys.png" alt="Diagram Venti Subsys"/>
      </p>
      </li>  
    </ul>
</ul>
