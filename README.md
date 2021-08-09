# IoT-Draw-Bot
### 🌅 Overview
High School (Year 11) Systems Engineering Project by Liam Price. A small handheld drawing robot that can be controlled through the internet. I made two models as gifts for friends back in 2019.

### 🤖 Physical Build Details
The physical build was made as a 3D model and printed on a FlashForge 3D Printer. The electrical components used include 2 servo motors for the wheels, 1 servo motor for the pen actuator, 1 lithium-ion battery, 1 Wemos D1 Mini Espressif Arduino board, battery charging components and a wireless charging coil (2019 model) / magnetic charger module (2020) I also melted and forged led to sit inside of the robot to help give it more weight so the pen could actually draw on paper. I used small earth magnets to keep the front wheel ball bearing in place for light and easy rotation while drawing.

### 💻 Software Overview
Built with Visual Micro in Visual Studio 2019. The libraries used are ArduinoFirebase, ESP8266 Community, WifiManager, PubSubClient, and the Arduino library.
The main c++ program can be found in the .ino file and the header files are located in the include folder. The website files such as the html and javascript file can be found in the web-server folder. You can also find the electrical schematics and 3D print STL files in this repository (built with ProDesktop 8.0)

### 🤔 Current Status
I originally built this project when I was pretty inexperienced at software development, therefore the original program was written horribly. The code that you will find in this current repository is a re-write of what I originally wrote back in 2019. At the time of writing (9-08-2021) the current main branch is not yet functional as I am making the transition from using Firebase to MQTT for this project, however, you can still successfully compile the code on to a Espressif board such as a Wemos D1 Mini, it just won't really do anything. If you would like to see the TERRIBLE (yet functional) original software I wrote for this back in 2019, you can view it [here](https://pastebin.com/9us41h7V)

### 📷 Photos and Videos
First model: (2019 Wireless Charger)
<p float="left">
  <img src="https://user-images.githubusercontent.com/51737378/128665286-de23d837-5022-4ab5-a613-64fb8ce3b997.JPG" width="320" />
   <img src="https://user-images.githubusercontent.com/51737378/128665290-6e51ac2a-56b4-4ccb-ad4c-40faae3dbd31.JPG" width="320" />
   <img src="https://user-images.githubusercontent.com/51737378/128691948-b80edcfe-3967-469b-ba9c-4929d64bef6f.png" width="320" />
   <img src="https://user-images.githubusercontent.com/51737378/128692017-f1cc4af6-696c-4309-980f-a65410fbace5.png" width="320" />
  <img src="https://user-images.githubusercontent.com/51737378/128692606-4df3639e-9f50-41cd-8a7f-c388b0d18281.png" width="320" />
  <img src="https://user-images.githubusercontent.com/51737378/128696378-f11e7eb8-1a4f-4bfd-8858-d32c4b7e8827.png" width="320" />

</p>

https://user-images.githubusercontent.com/51737378/128624825-71ae2798-04d4-4aa1-82b3-56bfeccb1233.mov

Second model: (2020 Magnetic Charger + other improvements)
<p float="left">
  <img src="https://user-images.githubusercontent.com/51737378/128622040-1bd8ef5b-ab07-4e1c-9267-39432def6d94.png" width="320" />
   <img src="https://user-images.githubusercontent.com/51737378/128623605-bbbd570f-2dd7-458c-8ce0-e44834441bb2.jpg" width="320" />
   <img src="https://user-images.githubusercontent.com/51737378/128623658-7682f233-a0c6-432a-9763-d50478c69d6a.JPG" width="320" />
   <img src="https://user-images.githubusercontent.com/51737378/128623663-c20e95cd-c811-459e-bc16-8549e676f1fc.JPG" width="320" />
</p>




