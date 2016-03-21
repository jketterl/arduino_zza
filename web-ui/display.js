var SerialPort = require('serialport').SerialPort,
    util = require('util');

var lines = require('../lines.json'),
    destinations = require('../destinations.json');

var Display = function(tty) {
    var me = this;
    me.openQueue = [];
    me.current = {
        line: '',
        destination: '',
        length:'lang',
        position:'mitte'
    };

    me.output = new SerialPort(tty, {
        baudrate: 9600
    }, true, function(err) {
        if (err) util.log(err.stack);
    });

    me.output.on('open', function(){
        util.log('serial connection opened');
        me.output.on('data', function(data) {
            util.log('serial data in: ' + data);
        });
        var queue = me.openQueue;
        delete me.openQueue;
        me.home(function(){
            queue.forEach(function(f){
                f();
            });
        });
    });

};

Display.prototype.onOpen = function(f) {
    if (this.openQueue) return this.openQueue.push(f);
    f();
};

Display.prototype.home = function(cb) {
    var me = this;
    me.onOpen(function(){
        util.log('homing display...');
        me.output.write('home\n', function(err) {
            if (err) util.log(err.stack);
        });
        // TODO implement wait for OK
        setTimeout(cb, 5000);
    });
};

Display.prototype.show = function(train, cb) {
    var me = this;

    if (me.timeout) clearTimeout(me.timeout);

    if (typeof(train.line) !== 'undefined' && typeof(lines[train.line]) !== 'undefined') {
        me.current.line = train.line;
    }
    if (typeof(train.destination) !== 'undefined' && typeof(destinations[train.destination]) !== 'undefined') {
        me.current.destination = train.destination;
    }
    if (typeof(train.length) !== 'undefined') {
        me.current.length = train.length;
    }
    if (typeof(train.position) !== 'undefined') {
        me.current.position = train.position;
    }

    
    me.onOpen(function(){
        var positionVariations = lines[me.current.line][me.current.length];
        var lineIndex = positionVariations;
        if (typeof(positionVariations) === 'object') {
            lineIndex = positionVariations[me.current.position];
        }

        var message = destinations[me.current.destination] + ',' + lineIndex + '\n';
        me.output.write(message, function(err) {
            if (err) return cb(err);
            cb();
        });

        me.timeout = setTimeout(function(){
            me.randomize();
        }, 60000);
    });
};

Display.prototype.randomize = function(){
    var me = this;
    var randomOffset = function(items) {
        return items[Math.floor(Math.random() * items.length)];
    };

    me.show({
        line: randomOffset(Object.keys(lines)),
        destination: randomOffset(Object.keys(lines)),
        length: randomOffset(['kurz', 'voll', 'lang']),
        position: randomOffset(['hinten', 'mitte', 'vorne'])
    }, function() {} );
};

module.exports = Display;
