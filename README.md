# Twingo Phase II steering wheel to Sony HU

My Twingo steering wheel remote did not go along with the new Sony MEX-N5300BT radio. But since it is a very handy gimmick and seemingly all new radios have tiny ugly buttons, I built an adapter to connect the two. Here's how:

## Resources
For the remote to arduino and the code part, my approach is heavily based on [this Reddit post](https://www.reddit.com/r/diyelectronics/comments/413liz/renault_steering_wheel_remote_to_jvc/) by Bart Kuijper, who Built an adapter for his JVC radio. He also drew a [schematic](https://imgur.com/vg5zvh5) and wrote a nice [arduino sketch](https://pastebin.com/fXbScxV4). JVC uses some sort of simple digital protocol, while Sony (and Pioneer) radios listen to resistances. Certain resistances trigger functions on the radio. For the resistances, there is a [nice post](http://jvde.net/comment/150) on jvde.net describing how Sony/Pioneer radios are controlled by resistance ladders.  
It seems that all base functions/resistances work the same across Sony and Pioneer, while anything phone specific (call/reject/voice control) does not. Trial and error..

## Remote to Arduino and power
The remote is solely connected through a male Mini-ISO plug (yellow), which fits in the factory radio. For testing, the remote can be removed from the car with a few screws. I removed it eventually, it saved a lot of times running to the car. I got a female Mini-ISO plug from ebay which connects the  remote to Arduino.  
The Arduino is powered via USB from a cigarette lighter socket to USB adapter. I used a cable with a right-angled USB-A plug that leads to the ashtray and down the console to the Arduino (if you remove the ashtray, you can lead the cable into the console and then hook the ashtray back in again).  
This way, you don't have to mingle with the car electrics and the whole operation can be removed without traces. Also, no need to worry about surges, a step down converter, etc. I stuffed everything in the console, around the radio there is enough room. 

![](/docs/twingo-sony-power.jpeg)

## Arduino to radio
I used 5V reed relays to open the circuit between tip/ring and sleeve/ring of the 3,5mm headphone jack that goes to the radio. Each relay is connected to a resistor for it's corresponding function. My first thought was to use optocouplers, but their inner resistance that also varies with temperature would complicate things. My radio uses a voltage of 3,3V between tip and ring (older models seem to use 5V), which leaves not much space to match the correct resistance that is needed to trigger a function. Those relays are relatively expensive (~2€ each), but they feature very low level of inner resistance and are fun to work with. I started a discussion about the right choice of components [here](https://www.mikrocontroller.net/topic/479725#new) (german). Also, have a look on the post and especially the comments in the JVDE post mentioned above about the resistance values.

### Code
The Arduino sketch is straightforward and well documented. Here is my setup:

| BUTTON | ARDUINO PIN| RESISTOR | HU FUNCTION |
|:--|:--|:--|:--|
| top left | A0 | 1,2k | standby/off |
| top right | A5 | Tip + Ring | voice control |
| bottom | A2 | 2,2K | source |
| vol + | A3 | 15K | vol + |
| mute | A1 | 4,7K | mute |
| vol - | A4 | 22K | vol - |
| wheel up | D10 | 5,6k + Ring | radio: preset + |
| wheel dn | D11 | 8,2k + Ring | radio: preset - |

Note that voice control only needs tip and ring connected to sleeve, there's no resistor for this function (found out by accident).

## Schematic and circuit
This is my circuit (not the final version). Have a look at the schematics and the PCB view of the Fritzing file. The circuit is a good base to start soldering, although I had to leave more space between the relays.  
![](/docs/twingo-sony-circuit-board.jpg)

As you can see in the PCB, the circuits to the Arduino are not routed. This is just for readability, I hand-soldered anyways.

![](/docs/twingo-sony-pcb.png)  