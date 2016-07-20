module.exports = [{
  func: require('./src/windows'),
  description: 'Generate React Native Windows template project',
  name: 'windows [name]',
  options: [{
    flags: '--windowsVersion <version>',
    description: 'The version of react-native-windows to use.'
  }, {
    flags: '--namespace <namespace>',
    description: 'The native project namespace.'
  }]
}];
