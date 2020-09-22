FROM mcr.microsoft.com/windows:2004
RUN mkdir c:\\setup
WORKDIR c:\\setup
COPY rnw-dependencies.ps1 .
Run powershell Set-Executionpolicy Unrestricted && powershell ./rnw-dependencies.ps1 -Install -NoPrompt buildlab

CMD c:\\windows\\system32\cmd.exe