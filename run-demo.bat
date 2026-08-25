@echo off
setlocal
chcp 65001 > nul
cd /d "%~dp0"
set "MAVEN_OPTS=--enable-native-access=ALL-UNNAMED --sun-misc-unsafe-memory-access=allow -Dorg.slf4j.simpleLogger.defaultLogLevel=warn"

echo ===================================================
echo  Building FastHTML ^& Launching Live AVX2 Demo
echo ===================================================

call "C:\Users\andre\tools\apache-maven-3.9.9\bin\mvn.cmd" -q -Dorg.slf4j.simpleLogger.defaultLogLevel=warn test-compile 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

call "C:\Users\andre\tools\apache-maven-3.9.9\bin\mvn.cmd" -q exec:java "-Dexec.mainClass=fasthtml.Demo" "-Dexec.args=" "-Dexec.vmArgs=--enable-native-access=ALL-UNNAMED" 2>nul
pause
