# <p align="center"><img src="zmote-logo3.png"></p>
###### <p align="right">[Click for Docs](https://github.com/AumBhatt/aura-zmote/raw/main/API-iTach.pdf)</p>
## 1. IP > IR
Compile: test/i1.cpp
```bash
g++ -g -o i1 i1.cpp -lpthread
```
`or` use makefile: test/makefile
```bash
cd test
make
```
Program Run:
#### 1. Learn Mode
Note: Doesn't Store Commands
```bash
./iX <IRX-ip-addr> <IRX-port>
```

#### 2. Test server: /server/server_ir.js
###### `Node.js 12.x+`
```bash
node server_ir
```
