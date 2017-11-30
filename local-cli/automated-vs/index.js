'use-strict';

const WPFProject = require('./projects/wpfProject');
const UWPProject = require('./projects/uwpProject');
const VS2017 = require('./visualStudios/vs2017');
const VS2015OrEarlier = require('./visualStudios/vs2015OrEarlier');
const getConfiguration = require('./utils/cliOptions').getConfiguration;
const getVerboseCLP = require('./utils/cliOptions').getVerboseCLP;

module.exports = {
    WPFProject,
    UWPProject,
    VS2017,
    VS2015OrEarlier,
    getConfiguration,
    getVerboseCLP
};
