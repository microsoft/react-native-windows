module.exports = [{
  func: require('./src/windows'),
  description: 'Generate React Native Windows template project',
  name: 'windows [name]',
  options: [{
    command: '--windowsVersion [version]',
    description: 'The version of react-native-windows to use.',
  }, {
    command: '--namespace [namespace]',
    description: 'The native project namespace.',
  }, {
    command: '--verbose',
    description: 'Enables logging.',
    default: false,
  }, {
    command: '--template [template]',
    description: 'Template to install. E.g., `vnext`.',
  }, {

    command: '--language [language]',
    description: 'Which language the template app is written in.  Possible values are cs and cpp',
    default: 'cpp',
  }]

    command: '--overwrite',
    description: 'Overwrite any existing files without prompting',
    default: false,
  }],

},{
  func: require('./src/wpf'),
  description: 'Generate React Native Windows template project on WPF',
  name: 'wpf [name]',
  options: [{
    command: '--windowsVersion [version]',
    description: 'The version of react-native-wpf to use.',
  }, {
    command: '--namespace [namespace]',
    description: 'The native project namespace.',
  }, {
    command: '--verbose',
    description: 'Enables logging',
    default: false,
  }],
},
];
