let config = {};
Object.assign(config, require('./core/windows'));
Object.assign(config, require('./link'));

module.exports = { windows: config };
