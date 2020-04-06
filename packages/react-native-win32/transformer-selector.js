// For React Native version 0.59 or later
var upstreamTransformer = require('metro-react-native-babel-transformer');
var svgTransformer = require('react-native-svg-transformer');

module.exports.transform = function({ src, filename, options }) {
if (filename.endsWith('.svg')) {
    return svgTransformer.transform({ src, filename, options });
  }  else {
    return upstreamTransformer.transform({ src, filename, options });
  }
};
