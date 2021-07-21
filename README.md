# <p align="center"><img src="zmote-logo3.png"></p>
## 1. IP > IR
Compile:
```bash
g++ -o zmir1 zmir1.cpp -lPocoNet -lPocoUtil -lPocoFoundation -lpthread
```
Program Run:
1. Send Command
```bash
./zmir1 <zmote-ip-addr> <device-uuid> -control <mod-freq> <mark-space-timing>
```
2. Learn Mode
<br>	Note: Doesn't Store Commands
```bash
./zmir1 <zmote-ip-addr> <device-uuid> -learn
```

## 2. IP > SERIAL

Compile:
```bash
g++ -o gcs1 gc_s1.cpp -lPocoNet -lPocoUtil -lPocoFoundation
```
Program Run:
1. get_SERIAL
```bash
./gcs1 <zmote-ip-addr> get_SERIAL
```
2. set_SERIAL
```bash
./gcs1 <zmote-ip-addr> set_SERIAL <baudrate> <flowcontrol> <parity>
```

Todo : https://github.com/AumBhatt/aura-zmote/issues/1#issue-941011234
