# IoT Graduation Board

## The idea
When you graduate from an Argentinian university, it's a tradition to throw eggs at the newly graduated and take pictures holding some kind of board with images and text, like this ones:

![rsz_1image_1](https://user-images.githubusercontent.com/75386425/148237199-1d2c5350-5cfb-4fdb-b7be-8395bbc1f711.png)

I graduated from the University of Buenos Aires in December 2021 as an Electronic Engineer and I wanted to do something a little more **challenging** than gluing pictures to cardboard. So I came with the idea to add some kind of display so that my friends and family that couldn't come to my graduation could send me some congratulatory messages (or whatever they wanted).

Looking for boards and parts that I had laying around, I came up with the following idea:

I need some kind of server to host a super simple webpage with a form for leaving a message, including the sender name and the message itself. This message will be sent to an ESP32 board via MQTT (thanks to my phone's hotspot) and it will be displayed throw a simple character display.

The most important thing was to stick with the KISS principle and make everything work in less than 6 hours (since I was also preparing the final presentation for the Uni) for free/cheap.

## The build
After I came up with this idea I started thinking about how to make it super simple since it was a one-off project. I played before with [Node-Red](https://nodered.org/) and I knew that you could make dashboards very easily, so I started looking for already hosted, ready-to-go solutions but non of the available seemed free ([some](https://fred.sensetecnic.com/) were free but didn't allow dashboards). 

### AWS
So I decided to open an [AWS](https://aws.amazon.com/) account, and use an EC2 server. If the account is new, like in my case, you have a one-year free tier for a t2.micro server ( 1vCPU and 1GB, more than enough for this project). I created a new instance running Ubuntu.

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


