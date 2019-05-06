module.exports = function makeProjectPatch(windowsConfig) {

  const projectInsert = `<ProjectReference Include="..\\${windowsConfig.relativeProjPath}">
      <Project>{${windowsConfig.projectGUID}}</Project>
      <Name>${windowsConfig.projectName}</Name>
    </ProjectReference>
    `;

  /* eslint-disable no-control-regex */
  return {
    pattern: '<ProjectReference Include="..\\..\\node_modules\\react-native-windows\\ReactWindows\\ReactNative\\ReactNative.csproj">',
    patch: projectInsert,
    unpatch: new RegExp(`[^\S\r\n]+<ProjectReference.+\\s+.+\\s+.+${windowsConfig.projectName}.+\\s+.+\\s`),
  };
};
