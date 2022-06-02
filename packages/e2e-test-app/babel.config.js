module.exports = {
  presets: ['module:metro-react-native-babel-preset'],
  plugins: [["@babel/plugin-proposal-private-methods", { "loose": true }],["@babel/plugin-proposal-private-property-in-object", { "loose": true }]],
};