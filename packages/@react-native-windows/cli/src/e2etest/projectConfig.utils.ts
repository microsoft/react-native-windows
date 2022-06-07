import fs from '@react-native-windows/fs'
import path from 'path';
import { copyAndReplace } from '../generator-common';

export const templateRoot = path.resolve('../../../vnext/template');

export const testProjectGuid = '{416476D5-974A-4EE2-8145-4E331297247E}';

export async function tryMkdir(dir: string): Promise<void> {
  try {
    await fs.mkdir(dir, {recursive: true});
  } catch (err) {}
}

export async function ensureCppAppProject(folder: string, name: string, useWinUI3?: boolean, useHermes?: boolean, useExperimentaNuget?: boolean) {
  const windowsDir = path.join(folder, 'windows');
  if (fs.existsSync(windowsDir)) {
    await fs.rmdir(windowsDir, {recursive: true});
  }
  await tryMkdir(windowsDir);

  const replacements = {
    name,
    namespace: name,
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
    useWinUI3: !!useWinUI3,
    useHermes: !!useHermes,
    useExperimentalNuget: !!useExperimentaNuget,
  };

  await copyAndReplace(
    path.join(templateRoot, 'cpp-app/proj/MyApp.sln'),
    path.join(windowsDir, `${name}.sln`),
    replacements,
    null
  );

  const projDir = path.join(windowsDir, name);
  await tryMkdir(projDir);

  await copyAndReplace(
    path.join(templateRoot, 'cpp-app/proj/MyApp.vcxproj'),
    path.join(projDir, `${name}.vcxproj`),
    replacements,
    null
  );

  await copyAndReplace(
    path.join(templateRoot, 'shared-app/proj/ExperimentalFeatures.props'),
    path.join(windowsDir, 'ExperimentalFeatures.props'),
    replacements,
    null
  );
}

export async function ensureCSharpAppProject(folder: string, name: string, useWinUI3?: boolean, useHermes?: boolean, useExperimentaNuget?: boolean) {
  const windowsDir = path.join(folder, 'windows');
  if (fs.existsSync(windowsDir)) {
    await fs.rmdir(windowsDir, {recursive: true});
  }
  await tryMkdir(windowsDir);

  const replacements = {
    name,
    namespace: name,
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
    useWinUI3: !!useWinUI3,
    useHermes: !!useHermes,
    useExperimentalNuget: !!useExperimentaNuget,
  };

  await copyAndReplace(
    path.join(templateRoot, 'cs-app/proj/MyApp.sln'),
    path.join(windowsDir, `${name}.sln`),
    replacements,
    null
  );

  const projDir = path.join(windowsDir, name);
  await tryMkdir(projDir);

  await copyAndReplace(
    path.join(templateRoot, 'cs-app/proj/MyApp.csproj'),
    path.join(projDir, `${name}.csproj`),
    replacements,
    null
  );

  await copyAndReplace(
    path.join(templateRoot, 'shared-app/proj/ExperimentalFeatures.props'),
    path.join(windowsDir, 'ExperimentalFeatures.props'),
    replacements,
    null
  );
}

export async function ensureWinUI3Project(folder: string) {
  await ensureCppAppProject(folder, 'WithWinUI3', true, false, false);
}

export async function ensureHermesProject(folder: string) {
  await ensureCppAppProject(folder, 'WithHermes', false, true, false);
}

export async function ensureExperimentalNuGetProject(folder: string) {
  await ensureCppAppProject(folder, 'WithExperimentalNuGet', false, false, true);
}