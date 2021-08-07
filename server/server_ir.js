const net = require('net');
const { setTimeout } = require('timers');
const PORT = 4998;

/*
 * let command =
 *     "sendir,1:1,0,37000,1,1,166,167,20,63,20,64,19,64,19,23,19,22,20,23,19,23,19,23,19,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,22,20,64,19,23,19,23,19," +
 *     "23,19,23,19,23,19,23,19,63,20,23,19,64,19,63,20,64,19,63,20,64,19,63,20,1729,166,167,20,64,19,64,19,64,19,23,19,23,19,23,19,23,19,23,19,64,19,64,19,64,19," + 
 *     "23,19,23,19,23,19,23,19,23,21,21,19,64,19,23,19,23,19,23,19,23,19,23,19,23,19,64,19,23,19,63,20,63,20,64,19,63,20,64,19,64,19,3692\r";
 */

let command = "sendir,1:3,1,38380,1,1,347,173,22,65,22,22,22,65,22,65,22,22,22,22,22,65,22,22,22,22,22,65,22,22,22,22,22,65,22,65,22,22,22,65,22,65,22,22,22,22,22,65,22,65,22,65,22,65,22,65,22,22,22,65,22,65,22,22,22,22,22,22,22,22,22,22,22,65,22,65,22,65,22,22,22,22,22,65,22,22,22,22,22,22,22,22,22,22,22,65,22,65,22,22,22,65,22,65,22,487\r"

const server = net.createServer((socket) => {
    console.log("New Client Connected");

    socket.on('data', (buffer) => {
        let data = '';
        data += buffer;
        console.log(data);
        if(data.includes("get_IRL")) {
            socket.write("IR Learner Enabled");
            for(var i=1; i<=4; ++i) {
                setTimeout(function() {
                    if(!socket.destroyed) {
                        console.log("> Sending command");
                        socket.write(command);
                    }
                }, 1000*i);
            }
/*             setTimeout(function() {
                socket.end("IR Learner Disabled");
                socket.destroy();
            }, 5000); */
        }
        else if(data.includes("stop_IRL")) {
            // socket.end("IR Learner Disabled");
            socket.destroy();
        }
    });

    socket.on('close', () => {
        console.log("Client Disconnected");
        socket.destroy();
    });

}).on('error', (err) => {
  // Handle errors here.
  throw err;
});

// Grab an arbitrary unused port.
server.listen(PORT, () => {
    console.clear();
  console.log('Server @\n', server.address());
});
