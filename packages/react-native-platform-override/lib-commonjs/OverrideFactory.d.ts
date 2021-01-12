/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import { CopyOverride, DerivedOverride, DirectoryCopyOverride, PatchOverride, PlatformOverride } from './Override';
import FileRepository, { ReactFileRepository } from './FileRepository';
/**
 * Allows creation of overrides, taking care of guts such as hashing, version checking, etc.
 */
export default interface OverrideFactory {
    createPlatformOverride(file: string): Promise<PlatformOverride>;
    createCopyOverride(file: string, baseFile: string, issue: number): Promise<CopyOverride>;
    createDerivedOverride(file: string, baseFile: string, issue?: number | 'LEGACY_FIXME'): Promise<DerivedOverride>;
    createPatchOverride(file: string, baseFile: string, issue: number | 'LEGACY_FIXME'): Promise<PatchOverride>;
    createDirectoryCopyOverride(directory: string, baseDirectory: string, issue: number): Promise<DirectoryCopyOverride>;
}
/**
 * Conrete implementation of an OverrideFactory
 */
export declare class OverrideFactoryImpl implements OverrideFactory {
    private reactRepo;
    private overrideRepo;
    constructor(reactRepo: ReactFileRepository, overrideRepo: FileRepository);
    createPlatformOverride(file: string): Promise<PlatformOverride>;
    createCopyOverride(file: string, baseFile: string, issue: number): Promise<CopyOverride>;
    createDerivedOverride(file: string, baseFile: string, issue?: number): Promise<DerivedOverride>;
    createPatchOverride(file: string, baseFile: string, issue: number | 'LEGACY_FIXME'): Promise<PatchOverride>;
    createDirectoryCopyOverride(directory: string, baseDirectory: string, issue: number): Promise<DirectoryCopyOverride>;
    private checkOverrideExists;
    private getBaseInfo;
}
