const fs = require('fs');
const path = require('path');
const glob = require('glob');

function projectConfigWindows(folder, userConfig = {}) {
  if (userConfig === null) {
    return null;
  }
  
  const sourceDir = userConfig.sourceDir || findWindowsAppFolder(folder);

  if (!sourceDir) {
    return null;
  }

  const projectSolution = findSolution(sourceDir);
  if (projectSolution){
    var extension = path.extname(projectSolution);
    var projectName = path.basename(projectSolution, extension);
  }
  const cppProjFile = findCppProject(sourceDir, projectName);
  const csProjectFile = findCSProject(sourceDir,projectName);

  return {
    sourceDir,
    projectSolution,
    projectName,
    cppProjFile,
    csProjectFile,
  };
}

function findWindowsAppFolder(folder) {
  const winDir = 'windows';
  const joinedDir = path.join(folder, winDir);
  if (fs.existsSync(joinedDir)) {
    return joinedDir;
  }

  return null;
}

function findSolution(folder) {
  const solutionPath = glob.sync(path.join('**', '*.sln'), {
    cwd: folder,
    ignore: ['node_modules/**', '**/Debug/**', '**/Release/**', 'Generated Files'],
  })[0];

  return solutionPath ? path.join(folder, solutionPath) : null;
}

function findCppProject(folder, projectName) {
  const cppProj = glob.sync(path.join('**', projectName + '.vcxproj'), {
    cwd: folder,
    ignore: ['node_modules/**', '**/Debug/**', '**/Release/**', '**/Generated Files/**', '**/packages/**'],
  })[0];

  return cppProj ? path.join(folder, cppProj) : null;
}

function findCSProject(folder, projectName) {
  const cppProj = glob.sync(path.join('**', projectName + '.csproj'), {
    cwd: folder,
    ignore: ['node_modules/**', '**/Debug/**', '**/Release/**', '**/Generated Files/**', '**/packages/**'],
  })[0];

  return cppProj ? path.join(folder, cppProj) : null;
}

module.exports = {
  projectConfigWindows: projectConfigWindows,
};
