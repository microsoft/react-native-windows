REM @echo off
setlocal
set containerDate=20200930.08
set containerCommit=d3a6c9fbf429adad31de2ae5679cba57de735a79

set prereqContainerName=rnwci.azurecr.io/pr/prereqs:%containerDate%
set warmedUpContainerName=rnwci.azurecr.io/pr/warmedup:%containerCommit%

pushd PreReq
copy ..\..\rnw-dependencies.ps1 .
docker build -m 12G -t %prereqContainerName% --compress .
popd

pushd WarmCache
docker build -m 12G --buildArg IMAGE=%prereqContainerName% -t %warmedUpContainerName% --compress .
popd