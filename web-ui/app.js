var express = require('express'),
    bodyParser = require('body-parser'),
    util = require('util'),
    Display = require('./display');

var lines = require('../lines.json'),
    destinations = require('../destinations.json');

var display = new Display('/dev/ttyACM0');

var app = express();
app.set('view engine', 'ejs');
app.use(express.static('public'));
var ueParser = bodyParser.urlencoded();

app.get('/', function(req, res) {
    res.render('index', {
        lines: Object.keys(lines),
        destinations: Object.keys(destinations)
    });
});

app.post('/update', ueParser, function(req, res) {
    display.show(req.body, function(err) {
        if (err) util.log(err);
        res.redirect('/');
    });
});

app.listen(80, function() {
    util.log('app started on port 80');
});
