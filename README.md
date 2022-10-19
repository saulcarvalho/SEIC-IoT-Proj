<h1> SEIC-IoT-Proj </h1>
<p align="justify"> <b><i>DISCLAIMER:</i></b> This project was solely developed by myself, for the class Electronic Systems for the Internet of Things, with components I had on the shelf. The model of the project, the main processing system and subsystems was my own call/choice. I have translated the C++ code presented in proj-code folder. All the images that I provide will appear in text in portuguese. Some items may appear partially finished.</p>

<p align="justify"> With the code I provide, you won't be able to single-handedly replicate the project, but it is a great help for similar projects. </p>

<h2> Description: </h2>
<p align="justify"> The objective of this project was to create a functioning system with multiple devices that communicated with each order in a typical IoT environment. Basically, the project is comprised of 1 main processing system and 2 subsystems, heating and ventilation: </p>
<ul>
  <li>
    <p align="justify"> The main processing system is composed of a Raspberry Pi 4 (connected to a router phisically with an ethernet cable), with 4 servers with different purposes. </p>
     <ul>
      <li> <p align="justify"> MQTT Broker Mosquitto - Allows the arrival of messages into the many MQTT existent MQTT topics; </p> </li>  
      <li> <p align="justify"> Data bridge Node-RED  - Allows the easy connection between the MQTT broker and the storage in the database; </p> </li>  
      <li> <p align="justify"> Real-time database InfluxDB - Allows the storage of all the incoming data in the respective fields inside a timetable, through database storage queries; </p> </li>  
      <li> <p align="justify"> Dashboard Grafana - Allows the querying of the database in order to pull the data and display it in a graphical, more appealing manner.</p> </li>  
     </ul>
  </li> 
  <li>
  <p align="justify">
    The heating subsystem is composed of a digital temperature sensor (DS18B20) and a 1 kW resistor (100 Ω) controlled by an ESP32 connected to a SSR.
    Low voltage is on the microcontroller side (3.3 VDC), optically isolated from the 'high voltage' side (230 VAC), which is the voltage applied to the resistor.
    A circuit breaker was also used on the AC side for extra protection of the load. The ESP32 was connected to the same network as the Raspberry Pi, but wirelessly, 
    through WiFi. </p>
  </li> 
  <li>
  <p align="justify">
    The ventilation subsystem is composed of an analog temperature sensor (LM35), a digital environmental sensor based on a BME680 IC, a 230 VAC fan controlled by an 
    ESP8266, also connected to a SSR. Low voltage is on the microcontroller side (3.3 VDC), optically isolated from the 'high voltage' side (230 VAC), which is the         voltage applied to the fan. A circuit breaker was also used on the AC side for extra protection of the load and a snubber circuit for the SSR high side protection     against high reverse voltage provoked by the fan's coils. The ESP8266 was connected to the same network as the Raspberry Pi, but wirelessly, through WiFi. </p>
  </li> 
</ul> 

<p align="justify"> A Telegram bot within a private group was created in order to receive commands and parametrize both subsystems wirelessly (through the same WiFi network), if desired. The temperature setpoint in both subsystems was configurable within a certain range as well as the upper and/or lower hysteresis for the ventilation subsystem. </p>

<h2> System Architecture </h2>
<p align="justify"> The control system of heating and ventilation is based on an IoT topology. It is composed by 2 subsystems, the heating subsystem and the ventilation subsystem. These 2 subsystem are connected to a WiFi network, in which there's a device that hosts multiple servers, this device encomprises the main processing system, this last allows the diffusion of the data obtained in the subsystems. The device used in this main system is a Raspberry Pi 4, and hosts a Node-RED that allows to create a data bridge between the data sent by the subsystem and the remaining services, like the MQTT broker Mosquitto server and the database server InfluxDB, which therefore communicates with a dashboard server, called Grafana. </p>

<p align="center"> 
    <img width="600" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/sys_arch.png" alt="Sys Architecture"/>
</p>


<h2> Project Components Summary </h2>
<p align="justify"> Project based in multiple hardware and software solutions:</p>
<h3 align="left"> Main Processing System </h3>
<p align="justify"> The Main Processing System, based on a Raspberry Pi 4 with Raspbian and with the following servers: </p> 
    <ul>
      <li> <p align="justify"> MQTT Broker - Mosquitto </p> </li>  
      <li> <p align="justify"> Data bridge - Node-RED </p> </li>  
      <li> <p align="justify"> Real-time database - InfluxDB </p> </li>  
      <li> <p align="justify"> Dashboard - Grafana </p> </li>  
      <p align="center"> 
        <img width="371" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/main_sys_1.png" alt="Main Processing Sys 1"/>
      </p>
    </ul>
    
<h3 align="left"> Heating Subsystem  </h3>
<p> Heating Subsystem, based on a Lolin NodeMCU V3 with a sensor shield. </p> 
   
<p align="center"> 
   <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/heat_subsys_1.png" alt="Heat Subsys 1"/>
   <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/heat_subsys_2.png" alt="Heat Subsys 2"/>
</p>
<p align="center"> 
   <img width="600" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/diagram_heat_subsys.png" alt="Diagram Heat Subsys"/>
</p>
   
     
<h3 align="left"> Ventilation Subsystem </h3>
<p align="justify"> Ventilation Subsystem, based on a WeMos LOLIN32 Lite (ESP32). </p> 

<p align="center"> 
    <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/venti_subsys_1.png" alt="Venti Subsys 1"/>
    <img height="200" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/venti_subsys_2.png" alt="Venti Subsys 2"/>
</p>
<p align="center"> 
    <img width="700" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/diagram_venti_subsys.png" alt="Diagram Venti Subsys"/>
</p>



<h2> Grafana screenshots </h2>
<p align="justify"> Below I present some of the grafana screenshots that I provided in my class report. </p>
<h3 align="center"> Grafana screenshot #1  </h3>
 <p align="center"> 
   <img height="400" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/grafana_print_1.png" alt="Grafana print 1"/>
 </p>
 <h3 align="center"> Grafana screenshot #2  </h3>
 <p align="center"> 
   <img height="300" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/grafana_print_2.png" alt="Grafana print 2"/>
 </p>
 <h3 align="center"> Grafana screenshot #3  </h3>
 <p align="center"> 
   <img height="400" src="https://github.com/saulcarvalho/SEIC-IoT-Proj/blob/main/assets/images/grafana_print_3.png" alt="Grafana print 3"/>
 </p>
