/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as t from 'io-ts';
/**
 * Serialized form of {@see PlatformOverride}
 */
declare const PlatformOverrideType: t.TypeC<{
    type: t.LiteralC<"platform">;
    file: t.StringC;
}>;
/**
 * Serialized form of {@see PatchOverride}
 */
declare const PatchOverrideType: t.TypeC<{
    type: t.LiteralC<"patch">;
    file: t.StringC;
    baseFile: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.UnionC<[t.NumberC, t.LiteralC<"LEGACY_FIXME">]>;
}>;
/**
 * Serialized form of {@see DerivedOverride}
 */
declare const DerivedOverrideType: t.TypeC<{
    type: t.LiteralC<"derived">;
    file: t.StringC;
    baseFile: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.UnionC<[t.UndefinedC, t.NumberC, t.LiteralC<"LEGACY_FIXME">]>;
}>;
/**
 * Serialized form of {@see CopyOverride}
 */
declare const CopyOverrideType: t.TypeC<{
    type: t.LiteralC<"copy">;
    file: t.StringC;
    baseFile: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.NumberC;
}>;
/**
 * Serialized form of {@see DirectoryCopyOverride}
 */
declare const DirectoryCopyOverrideType: t.TypeC<{
    type: t.LiteralC<"copy">;
    directory: t.StringC;
    baseDirectory: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.NumberC;
}>;
/**
 * Union of all serialized override types
 */
declare const OverrideType: t.UnionC<[t.TypeC<{
    type: t.LiteralC<"platform">;
    file: t.StringC;
}>, t.TypeC<{
    type: t.LiteralC<"patch">;
    file: t.StringC;
    baseFile: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.UnionC<[t.NumberC, t.LiteralC<"LEGACY_FIXME">]>;
}>, t.TypeC<{
    type: t.LiteralC<"derived">;
    file: t.StringC;
    baseFile: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.UnionC<[t.UndefinedC, t.NumberC, t.LiteralC<"LEGACY_FIXME">]>;
}>, t.TypeC<{
    type: t.LiteralC<"copy">;
    file: t.StringC;
    baseFile: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.NumberC;
}>, t.TypeC<{
    type: t.LiteralC<"copy">;
    directory: t.StringC;
    baseDirectory: t.StringC;
    baseVersion: t.StringC;
    baseHash: t.StringC;
    issue: t.NumberC;
}>]>;
/**
 * Schema for the "overrides.json" manifest
 */
declare const ManifestType: t.TypeC<{
    includePatterns: t.UnionC<[t.UndefinedC, t.ArrayC<t.StringC>]>;
    excludePatterns: t.UnionC<[t.UndefinedC, t.ArrayC<t.StringC>]>;
    overrides: t.ArrayC<t.UnionC<[t.TypeC<{
        type: t.LiteralC<"platform">;
        file: t.StringC;
    }>, t.TypeC<{
        type: t.LiteralC<"patch">;
        file: t.StringC;
        baseFile: t.StringC;
        baseVersion: t.StringC;
        baseHash: t.StringC;
        issue: t.UnionC<[t.NumberC, t.LiteralC<"LEGACY_FIXME">]>;
    }>, t.TypeC<{
        type: t.LiteralC<"derived">;
        file: t.StringC;
        baseFile: t.StringC;
        baseVersion: t.StringC;
        baseHash: t.StringC;
        issue: t.UnionC<[t.UndefinedC, t.NumberC, t.LiteralC<"LEGACY_FIXME">]>;
    }>, t.TypeC<{
        type: t.LiteralC<"copy">;
        file: t.StringC;
        baseFile: t.StringC;
        baseVersion: t.StringC;
        baseHash: t.StringC;
        issue: t.NumberC;
    }>, t.TypeC<{
        type: t.LiteralC<"copy">;
        directory: t.StringC;
        baseDirectory: t.StringC;
        baseVersion: t.StringC;
        baseHash: t.StringC;
        issue: t.NumberC;
    }>]>>;
}>;
export declare type PlatformOverride = t.TypeOf<typeof PlatformOverrideType>;
export declare type PatchOverride = t.TypeOf<typeof PatchOverrideType>;
export declare type DerivedOverride = t.TypeOf<typeof DerivedOverrideType>;
export declare type CopyOverride = t.TypeOf<typeof CopyOverrideType>;
export declare type DirectoryCopyOverride = t.TypeOf<typeof DirectoryCopyOverrideType>;
export declare type Override = t.TypeOf<typeof OverrideType>;
export declare type Manifest = t.TypeOf<typeof ManifestType>;
/**
 * Read an override manifest from a file.
 *
 * @throws if the file is invalid or cannot be found
 */
export declare function readManifestFromFile(filePath: string): Promise<Manifest>;
/**
 * Parse a string with JSON for the override manifest into one.
 *
 * @throws if the JSON doesn't describe a valid manifest
 */
export declare function parseManifest(json: string): Manifest;
/**
 * Writes the manifest to a JSON file. Does not validate correctness of the
 * manifest.
 */
export declare function writeManifestToFile(manifest: Manifest, filePath: string): Promise<void>;
export {};
