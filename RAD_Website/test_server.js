var http = require("http");
var url = require("url");
var fs = require("fs");
var s = require("net").Socket();


var server = http.createServer(function(req, resp) 
{
    var uri = url.parse(req.url).pathname;
    console.log(req.url);
	//RootHTML page
	
	if(uri === "/")
	{
		
		fs.readFile("./Format.html", function (error, pgResp) {
            console.log("currently in function Root");
            if (error) {
                resp.writeHead(404);
                resp.write('Contents you are looking are Not Found');
            }
            else {
                console.log("currently else or printing Root");
                resp.writeHead(200, { 'Content-Type': 'text/html' });
                resp.write(pgResp);
            }
            resp.end();
        });
	}
	//BasicHTML, for testing
    else if(uri === "/BASIC.html") 
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
	//TestHTML, for testing
    else if (uri === "/test") {
        fs.readFile("./testing.html", function (error, pgResp) {
            console.log("currently in function Testing");
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
	else
	{
        console.log("interesting, second else: " + uri);
	    resp.end();
	};
});

server.listen(4343);
console.log("Server is listening");
