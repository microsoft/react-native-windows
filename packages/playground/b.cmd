setlocal

pushd ..\..

call yarn || goto :fail

popd

call yarn start

endlocal

exit /b 0

:fail
echo ERROR, failed!
exit /b 1

