var SerialPort = require('serialport').SerialPort,
    util = require('util');

var lines = require('../lines.json'),
    destinations = require('../destinations.json');

var Display = function(tty) {
    var me = this;
    me.openQueue = [];
    me.current = {
        line: '',
        destination: ''
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
    if (typeof(lines[train.line]) === 'undefined' || typeof(destinations[train.destination]) == 'undefined') {
        return cb(new Error('train not displayable :('));
    }
    me.onOpen(function(){
        var message = lines[train.line] + ',' + destinations[train.destination] + '\n';
        me.output.write(message, function(err) {
            if (err) return cb(err);
            me.current = train;
            cb();
        });
    });
};

module.exports = Display;
