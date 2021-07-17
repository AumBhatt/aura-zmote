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
        res.write(handleZmoteRequest(requestBody));
        if(requestBody === 'stop_IRL') {
            res.end();
        }
        else {
            setTimeout(() => {
                res.write("\sample-ir-command");
                res.end();
            }, 5000);
        }
        setTimeout(() => {
            res.end();
        }, 20000);
    });
});

function handleZmoteRequest(requestBody) {
    switch(true) {
        case (requestBody === 'get_IRL'):
            return `IR Learner Enabled`;
        case (requestBody === 'stop_IRL'):
            return `IR Learner Disabled`;
        default:
            return "unknown";
    }
}

app.listen(port, () => {
    console.clear();
    console.log(`zmote server: Listening @ ${port}`);
});
