module.exports = {
  extends: "@rnw-scripts/babel-react-native-config",
  presets: [
    "module:metro-react-native-babel-preset"
  ],
  plugins: [
    "babel-plugin-transform-flow-enums"
  ]
};