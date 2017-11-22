// client
var s = require('net').Socket();

s.connect(4242,'127.0.0.1');

s.write('web');

s.on('data', function(d){
    console.log(d.toString());
});

s.end();