# ESP32-LIVENBA-LEDMATRIX
Build a 32x64 Live NBA desktop scoreboard using an ESP-32 min and a 32x64 LED Matrix panel.

Rapid API Basketball:
  Using API-BASKETBALL https://rapidapi.com/api-sports/api/api-basketball to       obtain live feed of scores. A subscription is required to update the data       based on the delay. 
Parts:
  ESP-32
    Amazon:  https://www.amazon.com/KeeYees-ESP8266-ESP-12F-Development-Compatible/dp/B08ZY7Q7TW/ref=pd_sbs_sccl_2_2/134-1453380-0613927?pd_rd_w=oZI2b&pf_rd_p=3676f086-9496-4fd7-8490-77cf7f43f846&pf_rd_r=PMVFN5EDY92BJR1FKNT8&pd_rd_r=3b018dab-eb14-462b-9565-1a0dcd9e2be1&pd_rd_wg=0oAPi&pd_rd_i=B08ZY7Q7TW&psc=1
  LED 64x32 Matrix:
    Amazon:  https://www.amazon.com/waveshare-Full-Color-Adjustable-Brightness-Displaying/dp/B09MB426K5/ref=sr_1_5?crid=3CQ8316P315JC&keywords=32x64+led+matrix&qid=1652112503&sprefix=32x64+LED+%2Caps%2C185&sr=8-5
  IR remote and reciever:
    Electro Peak: https://electropeak.com/ir-remote-control-kit
  5V 10A 50W Power Supply:
     Amazon: https://www.amazon.com/dp/B07CMM2BBR?ref=ppx_yo2ov_dt_b_product_details&th=1
     
 Wiring to ESP-32:
  LED Matrix --> ESP-32:

| R1  G1  | R1 -> IO25 G1 -> IO26 
    
| B1  GND | B1 -> IO27 

| R2  G2  | R2 -> IO14 G2 -> IO12 

| B2  E   | B2 -> IO13 E -> IO 

| A   B   | A -> IO23 B -> IO19 

| C   D   | C -> IO05 D -> IO17 

| CLK LAT | CLK -> IO16 LAT -> IO 4

| OE  GND | OE -> IO15 GND -> ESP32 GND

IR reciever --> ESP-32:

|Signal| -> IO33

| Vcc  | -> ESP32 3.3V

| GND  | -> ESP32 GND
 
 Powering the ESP-32:
  A standard Micro-USB plugged into a wall outlet using a standard USB wall     adapter
 
 Powering LED Matrix:
  Using 5V power adapter.
    
 
 
      
