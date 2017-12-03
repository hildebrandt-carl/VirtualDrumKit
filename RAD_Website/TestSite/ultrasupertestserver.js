var http = require("http");
var url = require("url");
var fs = require("fs");
var formidable = require("formidable");
var util = require('util');
var s = require("net").Socket();


var server = http.createServer(function(req, resp) 
{
    var uri = url.parse(req.url).pathname;
    console.log(req.url);
	console.log(req.method);
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
	
	if (req.method.toLowerCase() == 'post') {
        processFormFieldsIndividual(req, resp);
    }
});

function processFormFieldsIndividual(req, resp) {
    //Store the data from the fields in your data store.
    //The data store could be a file or database or any other store based
    //on your application.
	console.log("Individual Fields");
    var fields = [];
    var form = new formidable.IncomingForm();
    form.on('field', function (field, value) {
        console.log(field);
		console.log(value);
		if(value === "Create Pi1")
		{
			console.log("Received Value");
		}
		else if(value === "Create Pi2")
		{
			console.log("Received Value2");
		}
		else if(value === "Create Pi3")
		{
			console.log("Received Value3");
		}
		else if(value === "Create Pi4")
		{
			console.log("Received Value4");
		}
		else if(value === "Create Pi5")
		{
			console.log("Received Value5");
		}
		else if(value === "Create Pi6")
		{
			console.log("Received Value6");
		}
		else if(value === "Create Pi7")
		{
			console.log("Received Value7");
		}
        fields[field] = value;
    });
	/*
    form.on('end', function () {
        resp.writeHead(1000, {
            'content-type': 'text/plain'
        });
        resp.write('received the data:\n\n');
        resp.end(util.inspect({
            fields: fields
        }));
    });
	*/
    form.parse(req);
}
server.listen(4343);
console.log("Server is listening");
