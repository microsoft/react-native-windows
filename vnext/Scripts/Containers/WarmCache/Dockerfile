ARG IMAGE=rnwci.azurecr.io/pr/prereqs:20200923

FROM $IMAGE
WORKDIR c:\\setup
COPY WarmupCaches.ps1 .
Run powershell ./WarmupCaches.ps1

CMD c:\\windows\\system32\cmd.exe