var http = require("http");
var url = require("url");
var path = require("path");
var fs = require("fs");

var server = http.createServer(function(req, resp) 
{
    var uri = url.parse(req.url).pathname;
    console.log(req.url);
	if(uri === "/BASIC.html")
	{
		fs.readFile("./BASIC.html", function (error, pgResp)
		{
		    console.log("currently in function Basic");
			if(error) {
			    resp.writeHead(404);
			    resp.write('Contents you are looking are Not Found');
			}
			else {
			    console.log("currently else or printing Basic");
			    resp.writeHead(200, { 'Content-Type': 'text/html' });
			    resp.write(pgResp);
			}
		    resp.end();
		});
	}
    else if (uri === "/testing.html") {
        fs.readFile("./testing.html", function (error, pgResp) {
            console.log("currently in function testing");
            if (error) {
                resp.writeHead(404);
                resp.write('Contents you are looking are Not Found');
            }
            else {
                console.log("currently else or printing Testing");
                resp.writeHead(200, { 'Content-Type': 'text/html' });
                resp.write(pgResp);
            }
            resp.end();
        });
    }
    else if (uri === "/favicon.ico") {
	    console.log("Currently in Favicon.ico");
	}
	else
	{
        console.log("interesting, second else: " + uri);
	    resp.end();
	};
});

server.listen(4343);
console.log("Server is listening");
