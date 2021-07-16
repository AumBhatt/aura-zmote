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
        requestBody = requestBody.substring(4, requestBody.length);
        res.write(requestBody);
        res.end();
    });
});


app.listen(port, () => {
    console.clear();
    console.log(`zmote server: Listening @ ${port}`);
});