# Funktionsgenerator
Funktionsgenerator basierend auf dem DDS AD9833 mit einem Atmega328PB (Arduino) als Mikrocontroller und einem 1,8" TFT Display

Function generator with AD9833 DDS, Atmega328PB Microcontroller and a 1.8" TFT Display

## Bill of Materials
### AD9833 & µController Board

| Qty | Value | Device | Package | Description | Vendor | Price |
| ---: | :--- | :--- | :--- | :--- | :--- | ---: |
| 1 | | KMR-1.8 SPI ST7735R | | 1.8" TFT Display |  [eBay](https://www.ebay.de/sch/i.html?_from=R40&_trksid=m570.l1313&_nkw=1%2C8%22+tft+spi+st7735r&_sacat=0) | 2,88€ |
| 1 | | SPDT | | Toggle switch for Front Panel Mount | [eBay](https://www.ebay.de/sch/i.html?_from=R40&_trksid=p2380057.m570.l1313.TR12.TRC2.A0.H0.Xtoggle+switch+12mm.TRS0&_nkw=toggle+switch+12mm&_sacat=0) | 3,00€ |
| 1 |  | KY-040 | | Rotary Encoder mit Taster (Ausführung mit Gewinde am Schaft und Befestigungsmutter) |  [eBay](https://www.ebay.de/itm/Drehregler-Drehgeber-Rotary-Encoder-Arduino-KY-040-Potentiometer-Poti-Raspberry/252713917550?hash=item3ad6ec5c6e:g:pIYAAOSwn25aD28N)| 4€ |
| 1 | 1,3k | Resistor | 0603 | | | 0,01€
| 1 | 620 | Resistor | 0603 |||0,01€|
| 8 | 100n | Capacitor | 0603 | | | 0,01€
| 4 | 10k | Resistor | 0603 ||| 0,01€|
| 1 | 10n | Capacitor| 0603||| 0,01€ |
| 1 |16Mhz | Crystal | SMD HC-49/U-S | 16.000MHz QCS HC49/U-S SMD Crystal 16MHz, 20ppm, 2-Pin HC-49/U-S SMD | [RS Components](https://de.rs-online.com/web/p/quarzmodule/8149535/)|0,25€|
| 1 | 1k |Potentiometer|| for Front Panel Mount | [RS Components](https://de.rs-online.com/web/p/products/8427030/)| 5,88€|
| 1 | 25k | Potentiometer || for Front Panel Mount| [RS Components](https://de.rs-online.com/web/p/products/8427059/)| 5,85€|
| 2 | 22p | Capacitor | 0603 | || 0,01€|
| 1 | 24.0Mhz | CFPS-72 | 7 x 5 mm SMD | | [RS Components](https://de.rs-online.com/web/p/products/8141581/)|1,25€|
|1 | 330n | Capacitor | 0603 | || 0,01€|
| 1 | | AD9833BRMZ | MSOP-10 | Programmable Waveform Generator | [Mouser](https://www.mouser.de/ProductDetail/Analog-Devices/AD9833BRMZ?qs=BpaRKvA4VqFt3Bdw9RBKCQ%3D%3D&gclid=Cj0KCQjwjtLZBRDLARIsAKT6fXx8JGdQ5f7k4kLWN22pvajQKU_Hv3pxUgl8WFMx-up7iK0Du9SND08aAm8KEALw_wcB) or [eBay](https://www.ebay.de/sch/i.html?_osacat=0&_odkw=AD9833BRMZ&_from=R40&_trksid=m570.l1313&_nkw=AD9833BRMZ+msop&_sacat=0)|7,79€|
| 1 | | ATMEGA328PB-AU | TQFP-32 | ATMEL - ATMEGA328PB-AU - MCU, 8BIT, ATMEGA, 20MHZ | [RS Components](https://de.rs-online.com/web/p/products/1468920/)|1,61€|
| 1 | 50Ohm | BNC Connector | | for Front Panel Mount | [RS Components](https://de.rs-online.com/web/p/products/5121174/)|2,15€|
| 1 | 10u | EEEFK1C100AR | Panasonic B | Panasonic 10uF 16 Vdc SMD Electrolytic Capacitor | [RS Components](https://de.rs-online.com/web/p/products/0565572/)|0,45€|
| 1 | 47u | Capacitor | Case D| Polarized Tantal Capacitor 10Vdc | [RS Components](https://de.rs-online.com/web/p/products/6993175/) | 0,36€|
| 2 | | LM318D | SO-08 | OpAmp | [RS Components](https://de.rs-online.com/web/p/products/6610524/) | 1,64€|
| 1 | | TLE4270-2D | TO-252 | LDO Voltage Regulator, 850mA, 5 V | [RS Components](https://de.rs-online.com/web/p/ldo-spannungsregler/1109087/) | 2,44€|


### Power Supply Board

| Qty | Value | Device | Package | Description | Vendor | Price |
| ---: | :--- | :--- | :--- | :--- | :--- | ---: |
|2 | 100n| Capacitor | 0603 | ||| 0,01€|
|1 | 2x12V 6VA | Transformer | | PCB Mount | [RS Components](https://de.rs-online.com/web/p/products/1213841/) | 5,75€|
|2 | 100u | EEEFT1C101AR | Panasonic C | Panasonic 100uF 16 Vdc SMD Electrolytic Capacitor | [RS Components](https://de.rs-online.com/web/p/aluminium-elektrolytkondensatoren/7472499/) | 0,35€|
|2 | 1000u/35V | Electrolyic Capacitor |E5-13| | [RS Components](https://de.rs-online.com/web/p/aluminium-elektrolytkondensatoren/7111337/) | 0,46€|
|1 | | MB6S| SOIC-4 | Bridge Rectifier | [RS Components](https://de.rs-online.com/web/p/products/6290322/)|0,35€|
| 1 |+12V| MC7812|DPAK| Positive Voltage Regulator| [RS Components](https://de.rs-online.com/web/p/products/6889250/)|0,70€|
|1|-12V| MC79M12|DPAK | Netgative Voltage Regulator|[RS Components](https://de.rs-online.com/web/p/products/6889326/)|0,65€|

#### Pin Connectors
You will need several pin headers and connectors , 2x2 Pin, 2x3 Pin, 1x 5 Pin. You could use any connector with a pitch of 0,1"/2,54mm either regular pinheaders or [Molex KK 254 Connectors](https://de.rs-online.com/web/c/steckverbinder/leiterplattensteckverbinder/leiterplatten-header/?redirect-relevancy-data=636F3D3126696E3D4931384E53656172636847656E65726963266C753D6465266D6D3D6D61746368616C6C7061727469616C26706D3D5E2E2A2426706F3D31333326736E3D592673723D52656469726563742673743D43415443485F414C4C5F44454641554C542673633D592677633D4E4F4E45267573743D73746966746C6569737465267374613D73746966746C656973746526&r=f&applied-dimensions=4294965668,4293240458,4294519411,4294519436,4294516238,4294878123).

### Images

![Bild](/Images/IMG_20180803_085027157.jpg)

![Bild](/Images/IMG_20180803_085035546.jpg)

![Bild](/Images/IMG_20180803_085046675.jpg)

This work is based on this [howto](https://www.allaboutcircuits.com/projects/how-to-DIY-waveform-generator-analog-devices-ad9833-ATmega328p/) from allaboutcircuits.com
