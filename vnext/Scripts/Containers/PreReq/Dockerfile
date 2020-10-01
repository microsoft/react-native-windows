# We have to use 1809 becuase the hosted VM's are Windows Server 2019 Core, and on windows the Kernels of the host and the container must match
FROM mcr.microsoft.com/windows/servercore:1809
RUN mkdir c:\\setup
WORKDIR c:\\setup
COPY rnw-dependencies.ps1 .
Run powershell Set-Executionpolicy Unrestricted && powershell ./rnw-dependencies.ps1 -Install -NoPrompt buildlab

CMD c:\\windows\\system32\cmd.exe