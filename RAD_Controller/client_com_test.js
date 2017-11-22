// client
var s = require('net').Socket();

s.connect(4242,'160.119.248.176');

s.write('Web');

s.on('data', function(d){
    console.log(d.toString());
});

s.end();