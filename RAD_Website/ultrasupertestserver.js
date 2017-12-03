var http = require("http");
var url = require("url");
var fs = require("fs");
var formidable = require("formidable");
var util = require('util');
var s = require("net").Socket();
var serverip = '160.119.248.28';
var serverport = 4242;

/*try{
	console.log("trying connection");
	s.connect(serverport, serverip);
	s.write("web");
}
catch(err){
	console.log("failed connection");
	resp.writeHead(420);
    resp.write('Failed Connection');
}*/

s.on('data', function(d){
	console.log(d.toString());
});

var server = http.createServer(function(req, resp) 
{
	
    var uri = url.parse(req.url).pathname;
    console.log(req.url);
	console.log(req.method);
	//RootHTML page
	if(uri === "/"){
		
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
	} else if(uri === "/form.html") {//FormHTML, for testing
        fs.readFile("./form.html", function (error, pgResp)
		{
		    console.log("currently in function Form");
			if(error) {
			    resp.writeHead(404);
			    resp.write('Contents you are looking are Not Found');
			}
			else {
			    console.log("currently else or printing Form");
			    resp.writeHead(200, { 'Content-Type': 'text/html' });
			    resp.write(pgResp);
			}
		    resp.end();
		});
    } else if (uri === "/test") {//TestHTML, for testing
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
    } else{
        console.log("interesting, second else: " + uri);
	    resp.end();
	};
	
	if (req.method.toLowerCase() == 'post') {
        processdrum(req, resp);
    }
});

function processdrum(req, resp) {
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
			//s.connect(serverport, serverip);
			s.write("a");
			//s.end();
		}
		else if(value === "Create Pi2")
		{
			console.log("Received Value2");
			//s.connect(serverport, serverip);
			s.write("b");
			//s.end();
		}
		else if(value === "Create Pi3")
		{
			console.log("Received Value3");
			s.write("c");
		}
		else if(value === "Create Pi4")
		{
			console.log("Received Value4");
			s.write("d");
		}
		else if(value === "Create Pi5")
		{
			console.log("Received Value5");
			s.write("e");
		}
		else if(value === "Create Pi6")
		{
			console.log("Received Value6");
			s.write("f");
		}
		else if(value === "Create Pi7")
		{
			console.log("Received Value7");
			s.write("g");
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
