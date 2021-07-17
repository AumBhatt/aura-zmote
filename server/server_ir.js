const express = require("express");
const app = express();
const port = 4998; // iTach Port number


app.all('*', (req, res) => {
    console.log("Request Endpoint : " + req.url);

    let requestBody = "";
    let responseBody;
    req.on('data', function(chunk) {
        requestBody += chunk;
    });
    req.on('end', function() {
        console.log("Request Body     : " + requestBody);

        if(requestBody.includes("sendir")){
            res.end("completeir,1:1,0").end();
        }
        else if(requestBody === 'stop_IRL') {
            res.write(`IR Learner Disabled`);
            res.end();
        }
        else if(requestBody === 'get_IRL'){
            res.write(`IR Learner Enabled`);
            setTimeout(() => {
                res.write(handleZmoteRequest(requestBody));
                res.write("\sample-ir-command");
                res.end();
            }, 5000);
        }
        else {
            res.end();
        }

        // Emulate no button pressed
        setTimeout(() => {
            res.end();
        }, 20000);
        console.log("Sending Response");
    });
});

app.listen(port, () => {
    console.clear();
    console.log(`zmote server: Listening @ ${port}`);
});
