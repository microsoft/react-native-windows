module.exports = {
  func: require('./src/wpf'),
  description: 'Generate React Native Windows template project on WPF',
  name: 'wpf [name]',
  options: [{
    command: '--windowsVersion [version]',
    description: 'The version of react-native-wpf to use.'
  }, {
    command: '--namespace [namespace]',
    description: 'The native project namespace.'
  }, {
    command: '--verbose',
    description: 'Enables logging',
    default: false,
  }]
};
