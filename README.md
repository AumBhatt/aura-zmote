## <p align="center">zmote</p>
Compile:
```
g++ -o zmir1 zmir1.cpp -lPocoNet -lPocoUtil -lPocoFoundation
```
Program Run:
1. Send Command
```
./zmir1 <zmote-ip-addr> <device-uuid> -control <mod-freq> <mark-space-timing>
```
2. Learn Mode
	Note: Doesn't Store Commands
```
./zmir1 <zmote-ip-addr> <device-uuid> -learn
```
Todo : https://github.com/AumBhatt/aura-zmote/issues/1#issue-941011234
