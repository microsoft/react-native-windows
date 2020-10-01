REM @echo off
setlocal
set containerDate=20201001.10
set containerCommit=ec54f8674c43d4fddc34f9e8ee9ce1b71124f572

set prereqContainerName=rnwci.azurecr.io/pr/prereqs:%containerDate%
set warmedUpContainerName=rnwci.azurecr.io/pr/warmedup:%containerCommit%

pushd PreReq
copy ..\..\rnw-dependencies.ps1 .
docker build -m 12G -t %prereqContainerName% --compress .
popd

pushd WarmCache
docker build -m 12G --build-arg IMAGE=%prereqContainerName% -t %warmedUpContainerName% --compress .
popd

call az acr login -n rnwci
docker push  rnwci.azurecr.io/pr/warmedup:%prereqContainerName%
docker push  rnwci.azurecr.io/pr/warmedup:%warmedUpContainerName%