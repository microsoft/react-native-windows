module.exports = function makeSolutionPatch(windowsConfig) {

  const solutionInsert = `Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "${windowsConfig.projectName}", "${windowsConfig.relativeProjPath}", "{${windowsConfig.projectGUID.toUpperCase()}}"
EndProject
`;

  return {
    pattern: 'Global',
    patch: solutionInsert,
    unpatch: new RegExp(`Project.+${windowsConfig.projectName}.+\\s+EndProject\\s+`, 'm'),
  };
};
