import fs from '@react-native-windows/fs'
import path from 'path';
import { copyAndReplace } from '../generator-common';

export const templateRoot = path.resolve('../../../vnext/templates');
export const sharedRoot = path.resolve('../../../vnext/template/shared-lib');

export const testProjectGuid = '{416476D5-974A-4EE2-8145-4E331297247E}';

export async function tryMkdir(dir: string): Promise<void> {
  try {
    await fs.mkdir(dir, { recursive: true });
  } catch (err) { }
}

export async function ensureCppAppProject(folder: string, name: string, useWinUI3?: boolean, useExperimentalNuget?: boolean) {
  const windowsDir = path.join(folder, 'windows');
  if (fs.existsSync(windowsDir)) {
    await fs.rmdir(windowsDir, { recursive: true });
  }
  await tryMkdir(windowsDir);

  const replacements = {
    name,
    namespace: name,
    useMustache: true,
    projectGuidUpper: testProjectGuid,
    projectGuidLower: testProjectGuid.toLowerCase(),
    useWinUI3: !!useWinUI3,
    useExperimentalNuget: !!useExperimentalNuget,
  };

  await copyAndReplace(
    path.join(templateRoot, 'cpp-app/windows/MyApp.sln'),
    path.join(windowsDir, `${name}.sln`),
    replacements,
    null
  );

  const projDir = path.join(windowsDir, name);
  await tryMkdir(projDir);

  await copyAndReplace(
    path.join(templateRoot, 'cpp-app/windows/MyApp/MyApp.vcxproj'),
    path.join(projDir, `${name}.vcxproj`),
    replacements,
    null
  );

  await copyAndReplace(
    path.join(sharedRoot, 'proj/ExperimentalFeatures.props'),
    path.join(windowsDir, 'ExperimentalFeatures.props'),
    replacements,
    null
  );
}

export async function ensureWinUI3Project(folder: string) {
  await ensureCppAppProject(folder, 'WithWinUI3', true, false);
}

export async function ensureHermesProject(folder: string) {
  await ensureCppAppProject(folder, 'WithHermes', false, false);
}

export async function ensureExperimentalNuGetProject(folder: string) {
  await ensureCppAppProject(folder, 'WithExperimentalNuGet', false, true);
}