'use-strict';

const WPFSolution = require('./solutions/wpfSolution');
const UWPSolution = require('./solutions/uwpSolution');
const getConfiguration = require('./utils/utility').getConfiguration;
const getVerboseCLP = require('./utils/utility').getVerboseCLP;

module.exports = {
    WPFSolution,
    UWPSolution,
    getConfiguration,
    getVerboseCLP
};
