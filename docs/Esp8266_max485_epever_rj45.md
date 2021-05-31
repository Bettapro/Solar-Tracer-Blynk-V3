# Connect  ESP8266  to EPEVER A/B Series ( + MAX485) 

Here the material involved:
* [ESP8266 Dev Board](https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20170114172938&SearchText=esp8266+mini)
* [EPSolar/EPEver Tracer A/B-Series](https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20170114172728&SearchText=tracer+mppt+rs485)
* [RS485 UART Module](https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20170114172807&SearchText=uart+rs485) (~~not the MAX485 chip!~~ - `@tekk:` I'm using [MAX485 cheapo module](../images/max485_module.jpg) and it works fine!)
* An old ethernet cable with RJ45 connector you are happy to cut open


## ESP8266 <-> MAX485 module

![This one worked for me](../images/max485_module.jpg)

I'm using this cheapo module and it works quite fine.
It's powered from `+5V` from ESP8266, and wired as following:


- `DI` -> `D10` / `GPIO1` / `TX`
- `RO` -> `D9` / `GPIO3` / `RX`
- `DE` and `RE` are interconnected with a jumper and then connected do eighter pin `D1` or `D2`
- `VCC` to `+5V` / `VIN` on ESP8266

	
![ESP8266 NodeMCU v0.9](../images/nodemcu_pins.png)

## MAX485 module <-> EPEVER CONTROLLER (RJ45)

Cut open your ethernet cable and split out pin 3, 5, 7 (B, A, GND). Refer to [Tracer Modbus PDF](../docs/1733_modbus_protocol.pdf) for additional info.

Connect cables as follows:
- Ethernet green, pin `5` -> `A`
- Ethernet blue, pin `3` -> `B`
- Ethernet brown, pin `7` -> `GND` on module **and** ESP8266 `GND` pin
		- -> to prevent ground loops - **important!**
- **DON' T** use pin `1` or `2` to feed the ESP8266 (they supply 7,5/5V - 50mA maximum)

![EPEVER RJ485 SPEC](../images/epever_rj45_specs.png)

