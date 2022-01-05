# IoT Graduation Board

## The idea
When you graduate from an Argentinian university, it's a tradition to throw eggs at the newly graduated and take pictures holding some kind of board with images and text, like this ones:

![rsz_1image_1](https://user-images.githubusercontent.com/75386425/148237199-1d2c5350-5cfb-4fdb-b7be-8395bbc1f711.png)

I graduated from the University of Buenos Aires in December 2021 as an Electronic Engineer and I wanted to do something a little **more challenging** than gluing pictures to cardboard. So I came with the idea to add some kind of display to the board so that my friends and family that couldn't come to my graduation could send me some congratulatory messages (or whatever they wanted) via a Webpage on the Internet. That's where I thought about building an IoT graduation board. 

![steps](https://user-images.githubusercontent.com/75386425/148269807-affdd3e9-012b-4b74-9428-6300ff7a4989.png)

Looking for boards and parts that I had laying around, I came up with the following idea:

![overall](https://user-images.githubusercontent.com/75386425/148261269-b8a7acaf-1f1e-4b1f-b8a3-d8e7809c90c3.png)

I need some kind of server to host a super simple webpage with a form for leaving a message, including the sender's name and the message itself. This message will be sent to an ESP32 board via MQTT (thanks to my phone's hotspot) and it will be displayed throw a simple character display.

The most important thing was to stick with the KISS principle and make everything work in less than 6 hours (since I was also preparing the final presentation for the Uni) for free/cheap.

## The build
After I came up with this idea I started thinking about how to make it super simple since it was a one-off project. I played before with [Node-Red](https://nodered.org/) and I knew that you could make dashboards very easily, so I started looking for already hosted, ready-to-go solutions but non of the available seemed free ([some](https://fred.sensetecnic.com/) were free but didn't allow dashboards). 

### AWS
So I decided to open an [AWS](https://aws.amazon.com/) account, and use an EC2 server. If the account is new, like in my case, you have a one-year free tier for a t2.micro server (1vCPU and 1GB, more than enough for this project). I created a new instance running Ubuntu.

![aws](https://user-images.githubusercontent.com/75386425/148239702-195b6678-064a-4c65-9ffb-bd3edd5d39f1.PNG)

After creating the credentials I was ready to SSH the server.

![ssh](https://user-images.githubusercontent.com/75386425/148240689-4601054d-c609-4ea1-8b66-c734ad64e703.PNG)

Then I installed Node-Red following the [instructions](https://nodered.org/docs/getting-started/aws#running-on-aws-ec2-with-ubuntu) and in a couple of minutes I had all working.

### Node-Red

The main idea of the flow is:

- Create an input form with the dashboard library, containing the sender's name, the message, and the SEND button.
- Validate the message (check for max length, remove accentuated characters) with a custom Javascript node.
- Add it to a FIFO queue for queueing messages for 15 seconds.
- Send the message topics via MQTT.

![node-red](https://user-images.githubusercontent.com/75386425/148241751-84dc7fbe-c759-4f0e-9fd5-7b9bcc521adc.PNG)

I build this with the available nodes, deploy it and test it with the debug node.
To make MQTT work, we need a broker. I installed [mosquitto](https://mosquitto.org/), a simple yet powerful MQTT broker that was running in the same EC2 instance. I configured the Node-Red's MQTT node to talk to the broker via localhost port 1883. You have to add Inbound Rules for the EC2 Instance to be able to use ports 1880 (Node-Red ui) and 1883 (MQTT).


After this, I started testing if everything was working with an MQTT testing app on my phone, reading the corresponding topics.

Simple things that I added to the flow:
- A pop-up message saying "Thank you" when the message was sent.
- All the messages were saved in a file (to read them afterward if I miss any).
- A field showing how many messages were in the queue.
- A keep-alive LED indication (kind of watchdog mechanism, didn't want to configure MQTT last will messages).

The final web page looked like this (accessible via EC2-INSTACE-PUBLIC-DOMAIN:1883/ui):

![node-red-ui](https://user-images.githubusercontent.com/75386425/148242027-5a046f43-1e06-4713-8ba7-97b77a88afdc.PNG)

### Hardware

I connected an ESP32 NodeMCU to an OLED character display. I looked online for a super simple MQTT script in Arduino and made the necessary changes to make it work with the rest of the system.

![LCD_NodeMCU_without_I2C (1)](https://user-images.githubusercontent.com/75386425/148262707-fb5aa917-37ba-4a5a-a1ab-da1c585012b1.png)

The firmware flow is:
- At boot, the ESP32 inits the display, searches the configured WiFi SSID, and connects to it ( phone's AP). Then it connects to the MQTT broker using the EC2's public domain.
- Displays a welcome message, indicating to scan the QR code on the board.
- Once an MQTT message arrives it gets parsed and the sender's name and message get printed in the character display in the correct positions.
- Every 5 seconds it sends a keep-alive message.
- If WiFi or MQTT's broker gets disconnected, reconnect.

Everything was powered-up by a 3000mAh power bank.

After some tests and fiddling around with the display configuration, I managed to make everything work. I solder the connections and sandwich everything to make it as thin as possible, to mount it to the board.

![image823](https://user-images.githubusercontent.com/75386425/148302009-2d9c377d-7a76-40e7-9157-5bc7849fb930.png)


## Results

![image823](https://user-images.githubusercontent.com/75386425/148268119-71f06a03-3208-4fa7-ac4a-5e1004761245.png)

The system worked without any problems! Some family members and friends that were at home or present at the moment sent their messages. 

The display looks bigger than the photos and is easy to read.

The board received some splash-damage though, getting a little bit yolked during the celebration :grin:.
