var http = require("http");

var server = http.createServer(function(request, response) {
    //var uri = url.parse(request.url).pathname;
    console.log(request.url);
    response.writeHead(200, {"Content-Type": "text/html"});
    response.write("<!DOCTYPE html>");
    response.write("<html>");
    response.write("<head>");
    response.write("<title>Hello World Page</title>");
    response.write("</head>");
    response.write("<body>");
    response.write("Hello World!");
    response.write("</body>");
    response.write("<button class='button' onclick=''> Green </button>");
    response.write("</html>");
    response.end();
});

server.listen(4343);
console.log("Server is listening");