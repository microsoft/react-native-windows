// .ado/scripts/build.js
// BinSkim security validation for React Native Windows.
//
// Can be run locally or in the Azure DevOps pipeline.
// Usage: node .ado/scripts/build.js --binskim [--platform x64] [--configuration Release] [--target desktop|universal]

import { execSync } from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import { parseArgs } from "node:util";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const sourcesPath = path.resolve(__dirname, "..", "..");

const binskimPackageName = "Microsoft.CodeAnalysis.BinSkim";
const binskimVersion = "4.4.9";
const binskimNuGetSource = "https://api.nuget.org/v3/index.json";

const options = {
  help:          { type: "boolean", default: false },
  binskim:       { type: "boolean", default: false },
  platform:      { type: "string",  default: "x64" },
  configuration: { type: "string",  default: "Release" },
  target:        { type: "string",  default: "" }, // "desktop", "universal", or "" for both
};

const args = parseArgs({ options, allowPositionals: false }).values;
if (args.help) { printHelp(); process.exit(0); }

main();

function main() {
  if (!args.binskim) {
    console.log("No action specified. Use --binskim to run BinSkim analysis.");
    console.log("Run with --help for usage details.");
    return;
  }

  const toolsPath = path.join(sourcesPath, "tools");
  const targetRoot = path.join(sourcesPath, "vnext", "target");
  const plat = args.platform;
  const config = args.configuration;

  // Collect binaries based on --target flag.
  const candidates = [];
  if (!args.target || args.target === "desktop") {
    candidates.push(
      path.join(targetRoot, plat, config, "React.Windows.Desktop.DLL", "react-native-win32.dll")
    );
  }
  if (!args.target || args.target === "universal") {
    candidates.push(
      path.join(targetRoot, plat, config, "Microsoft.ReactNative", "Microsoft.ReactNative.dll")
    );
  }

  runBinSkim({ toolsPath, candidates });
}

function printHelp() {
  console.log(`
Usage: node build.js --binskim [options]

Options:
  --binskim          Run BinSkim security analysis on shipped DLLs
  --platform <arch>  Build platform (default: x64)
  --configuration <cfg>  Build configuration (default: Release)
  --target <type>    Target to scan: desktop, universal, or omit for both
  --help             Show this help message
`);
}

function ensureDir(dirPath) {
  if (!fs.existsSync(dirPath)) {
    fs.mkdirSync(dirPath, { recursive: true });
  }
}

function ensureNuGet(toolsPath) {
  // Check if nuget.exe is already in the tools directory.
  const localNuGet = path.join(toolsPath, "nuget.exe");
  if (fs.existsSync(localNuGet)) {
    return localNuGet;
  }

  // Check if nuget.exe is in PATH.
  try {
    const result = execSync("where nuget.exe", { encoding: "utf8" }).trim();
    if (result) {
      const firstLine = result.split(/\r?\n/)[0];
      console.log(`Found nuget.exe in PATH: ${firstLine}`);
      return firstLine;
    }
  } catch {
    // Not in PATH, download it.
  }

  // Download nuget.exe from the official distribution.
  ensureDir(toolsPath);
  console.log(`Downloading nuget.exe to: ${localNuGet}`);
  execSync(
    `powershell.exe -NoLogo -NoProfile -Command ` +
      `"[Net.ServicePointManager]::SecurityProtocol = ` +
      `[Net.SecurityProtocolType]::Tls12; ` +
      `Invoke-WebRequest -Uri 'https://dist.nuget.org/win-x86-commandline/latest/nuget.exe' ` +
      `-OutFile '${localNuGet}' -UseBasicParsing"`,
    { stdio: "inherit" },
  );

  if (!fs.existsSync(localNuGet)) {
    throw new Error("Failed to download nuget.exe");
  }
  return localNuGet;
}

function runBinSkim({ toolsPath, candidates }) {
  // Determine BinSkim.exe path within the NuGet package.
  const binskimDir = path.join(toolsPath, "binskim");
  const binskimPkgDir = path.join(
    binskimDir,
    `${binskimPackageName}.${binskimVersion}`,
  );

  // Find BinSkim.exe under the package tools directory. The runtime folder
  // name varies by package version (e.g. netcoreapp3.1, net9.0).
  function findBinskimExe() {
    const toolsDir = path.join(binskimPkgDir, "tools");
    if (!fs.existsSync(toolsDir)) return null;
    for (const runtime of fs.readdirSync(toolsDir)) {
      const candidate = path.join(toolsDir, runtime, "win-x64", "BinSkim.exe");
      if (fs.existsSync(candidate)) return candidate;
    }
    return null;
  }

  let binskimExe = findBinskimExe();

  // Install BinSkim NuGet package if not already present.
  if (!binskimExe) {
    ensureDir(binskimDir);
    const nuget = ensureNuGet(toolsPath);
    console.log(
      `Installing ${binskimPackageName} ${binskimVersion} from ${binskimNuGetSource}...`,
    );
    execSync(
      `"${nuget}" install ${binskimPackageName}` +
        ` -Version ${binskimVersion}` +
        ` -Source "${binskimNuGetSource}"` +
        ` -OutputDirectory "${binskimDir}"`,
      { stdio: "inherit" },
    );
    binskimExe = findBinskimExe();
    if (!binskimExe) {
      throw new Error(
        `BinSkim.exe not found after install in: ${binskimPkgDir}`,
      );
    }
  }

  // Filter to binaries that exist on disk.
  const binaries = candidates.filter((f) => fs.existsSync(f));

  if (binaries.length === 0) {
    console.warn("BinSkim: No binaries found to scan. Skipping.");
    return;
  }

  console.log(`\nRunning BinSkim ${binskimVersion} on ${binaries.length} binaries:`);
  for (const b of binaries) {
    console.log(`  ${path.basename(b)}`);
  }

  const fileArgs = binaries.map((b) => `"${b}"`).join(" ");
  try {
    execSync(
      `"${binskimExe}" analyze` +
        ` --config default` +
        ` --ignorePdbLoadError` +
        ` --ignorePELoadErrors True` +
        ` --hashes` +
        ` --statistics` +
        ` --disable-telemetry True` +
        ` ${fileArgs}`,
      { stdio: "inherit" },
    );
    console.log("\nBinSkim: All rules passed.");
  } catch (error) {
    console.error(
      `\nBinSkim failed with exit code: ${error.status || "unknown"}`,
    );
    process.exit(error.status || 1);
  }
}
