# Twingo steering wheel to Sony HU

My Twingo steering wheel remote did not go along with the new Sony MEX-N5300BT radio. But since it is a very handy gimmick and seemingly all new radios have tiny ugly buttons, I built an adapter to connect the two. Here's how:

## Resources
For the remote to arduino and the code part, my approach is heavily based on [this Reddit post](https://www.reddit.com/r/diyelectronics/comments/413liz/renault_steering_wheel_remote_to_jvc/) by Bart Kuijper, who Built an adapter for his JVC radio. He also drew a [schematic](https://imgur.com/vg5zvh5) and wrote a nice [arduino sketch](https://pastebin.com/fXbScxV4). JVC uses some sort of simple digital protocol, while Sony (and Pioneer) radios listen to resistances. Certain resistances trigger functions on the radio.         For the resistances, there is a [nice post](http://jvde.net/comment/150) on jvde.net describing how Sony/Pioneer radios are controlled by resistance ladders.  
It seems that all base functions/resistances work the same across Sony and Pioneer, while everything phone specific (call/reject/voice control) does not. Trial and error..

## Remote to Arduino and power
The remote is solely connected by a male Mini-ISO plug (yellow), which fits in the factory radio. For testing, the remote can be removed from the car with a few screws. I removed it eventually, it saved a lot of times running to the car. I got a female Mini-ISO plug from ebay which connects the  the remote to Arduino.  
The Arduino is powered via USB from a cigarette lighter socket to USB adapter. I got a cable with a right angled USB-A plug that leads to the ash tray (remove the ash tray, insert the USB cable and mount it again) into the console and down to Arduino. This way you don't have to touch the car electrics and the whole operation can be removed without traces. Also, no need to worry about surges, a step down converter, etc. I stuffed everything in the console, next to the radio there is enough room.

## Arduino to radio
I used 5V reed relays to open the circuit between tip/ring and sleeve/ring of the 3,5mm headphone jack that goes to the radio. Each relay is connected to a resistor for it's corresponding function. My first thought was to use optocouplers, but their inner resistance that also varies with temperature would complicate things. My radio uses a voltage of 3,3V between tip and ring (older models seem to use 5V), which leaves not much space to match the correct resistance that is needed to trigger a function. Those relays are relatively expensive (~2€ each), but they feature very low level of inner resistance and are fun to work with. I started a discussion about the right choice of components [here](https://www.mikrocontroller.net/topic/479725#new) (german). Also, have a look on the post and especially the comments in the JVDE post mentioned above about the resistance values.

### Code
The Arduino sketch is straightforward and well documented. Here is my setup:

| BUTTON | RESISTOR | HU FUNCTION |
|:--|:--|:--|
| top left | 1,2k | standby/off |
| top right | Diode only | voice control |
| bottom | 2,2K | source |
| vol + | 15K | vol + |
| mute | 4,7K | mute |
| vol - | 22K | vol - |
| wheel up | 5,6k + Diode | radio: preset + |
| wheel dn | 8,2k + Diode | radio: preset - |

Note that voice control only needs tip and ring connected to sleeve, there's no resistor for this function (found out by accident).

## Schematic and circuit
![Circuit](/docs/twingo-sony-circuit-board.jpg) This is my circuit (not the final version). Have a look at the schematics and the PCB view of the Fritzing file. The circuit is a good base to start soldering, although I had to leave more space between the relays.  

![PCB](/docs/twingo-sony-pcb.png)  
As you can see in the PCB, the circuits to the Arduino are not routed. This is just for readability, I hand-soldered anyways.