# Openbaar vervoer "stop display" for M5Paper

![M5Paper](img/m5paper.jpg)

Displays a constantly updating real-time information from `ovapi.nl` for your desired tram/bus lines
using a M5Paper device. Uses a Cloudflare Worker to pre-process the information. 

## Requires

- M5Paper
- ESP-IDF 5.0

## Build

You'll need to deploy the Cloudflare worker in [rosmo/ovapi-cfworker](https://github.com/rosmo/ovapi-cfworker) with
your lines (eg. `GVB_LineNumber_Direction` and stop number (`TimingPointCode`)). Then run the configuration and
set your Cloudflare worker URL and WiFi details. To find your stop numbers, open the line URL with your
browser for the line, eg: `https://v0.ovapi.nl/line/GVB_5_2`, search for your stop name and note down the `TimingPointCode`.

```sh
idf.py menuconfig
idf.py build
idf.py -p com3 flash
```
