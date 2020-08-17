const fs = require('fs');
const path = require('path');
const glob = require('glob');
const xmldoc = require('xmldoc');

function dependencyConfigWindows(folder, userConfig = {}) {
  if (userConfig === null) {
    return null;
  }
  
  const sourceDir = userConfig.sourceDir || findWindowsAppFolder(folder);

  if (!sourceDir) {
    return null;
  }

  var packageName = null;
  const packageIDL = findPackageProviderIDL(sourceDir);
  if (packageIDL){
    packageName = parsePackageIDLFile(packageIDL);
  }
  var cppProjFile = null;
  var csProjectFile = null;
  if (packageName)
  {
    cppProjFile = findCppProject(sourceDir, packageName);
    csProjectFile = findCSProject(sourceDir,packageName);
  }

  var projGUID = null;
  if (cppProjFile)
  {
    const proj = readProject(cppProjFile);
    var groupNode = proj.childNamed('PropertyGroup');
    if (groupNode){
      var nameNode = groupNode.childNamed('ProjectGuid');
      if (nameNode){
        projGUID = nameNode.val;
      }
    }
  }

  return {
    sourceDir,
    packageIDL,
    packageName,
    cppProjFile,
    csProjectFile,
    projGUID,
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

// assumption is every cpp native module will have a ReactPackageProvider.idl defined
function findPackageProviderIDL(folder) {
  const PackageIDLPath = glob.sync(path.join('**', 'ReactPackageProvider.idl'), {
    cwd: folder,
    ignore: ['node_modules/**', '**/Debug/**', '**/Release/**', 'Generated Files'],
  })[0];

  return PackageIDLPath ? path.join(folder, PackageIDLPath) : null;
}

// look for packagename 'XYZ' in string 'namesapce XYZ {'
function parsePackageIDLFile(packageIDL) {
  const buf = fs.readFileSync(packageIDL, 'utf8');
  const indexofNameSpace = buf.indexOf('namespace') + 9;
  const indexofBracket = buf.indexOf('{');
  const packageName = buf.substring(indexofNameSpace, indexofBracket).replace(/\s+/g, ' ').trim();

  return packageName;
}

// read visual studio project file which is actually a XML doc
function readProject(projectPath) {
  return new (xmldoc.XmlDocument)(fs.readFileSync(projectPath, 'utf8'));
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
  dependencyConfigWindows: dependencyConfigWindows,
};
