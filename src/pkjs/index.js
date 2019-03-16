var Clay = require('pebble-clay');
var clayConfig = require('./config');
var validate = require('./validate');
var clay = new Clay(clayConfig, validate);
