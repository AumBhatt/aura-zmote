## <p align="center">zmote</p>
Compile:
```
g++ -o zmir1 zmir1.cpp -lPocoNet -lPocoUtil -lPocoFoundation
```
Program Run:
1. Learn Mode
```
./zmir1 <zmote-ip-addr> <device-uuid> -learn
```
2. Send Command
```
./zmir1 <zmote-ip-addr> <device-uuid> -control <mod-freq> <mark-space-timing>
```
Todo : https://github.com/AumBhatt/aura-zmote/issues/1#issue-941011234
