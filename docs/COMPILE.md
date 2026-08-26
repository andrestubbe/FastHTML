# Compiling FastHTML from Source

## Prerequisites

- **Windows 10 / 11 64-Bit**
- **JDK 17+** (JDK 26 recommended)
- **Visual Studio 2022 / 2026 Developer Command Prompt** (MSVC x64 compiler)
- **Apache Maven 3.9+**

## Build Steps

1. **Compile Native DLL**:
   ```cmd
   .\compile.bat
   ```
2. **Build Java JAR**:
   ```cmd
   mvn clean package -DskipTests
   ```
3. **Run Demo**:
   ```cmd
   .\run-demo.bat
   ```
