/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
/**
 * A subset of the per-instance properties returned by vswhere
 */
interface VisualStudioInstallation {
    instanceId: string;
    installationPath: string;
    installationVersion: string;
}
/**
 * Enumerate the installed versions of Visual Studio
 */
export declare function enumerateVsInstalls(opts: {
    requires?: string[];
    version?: string;
    verbose?: boolean;
    latest?: boolean;
}): VisualStudioInstallation[];
/**
 * Find the latest available VS installation that matches the given constraints
 */
export declare function findLatestVsInstall(opts: {
    requires?: string[];
    version?: string;
    verbose?: boolean;
}): VisualStudioInstallation | null;
export {};
