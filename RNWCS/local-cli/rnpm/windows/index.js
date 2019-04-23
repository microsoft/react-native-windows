module.exports = [{
  func: require('./src/windows'),
  description: 'Generate React Native Windows template project',
  name: 'windows [name]',
  options: [{
    command: '--windowsVersion [version]',
    description: 'The version of react-native-windows to use.'
  }, {
    command: '--namespace [namespace]',
    description: 'The native project namespace.'
  }, {
    command: '--verbose',
    description: 'Enables logging',
    default: false,
  }]
},{
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
}
];
