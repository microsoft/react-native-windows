exports.linkConfig = function() {
  return {
    isInstalled: require('./windows/isInstalled'),
    register: require('./windows/registerNativeModule'),
    unregister: require('./windows/unregisterNativeModule')
  };
};
