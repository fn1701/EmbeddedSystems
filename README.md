# Downlaod

```bash
git clone --recursive https://code.fbi.h-da.de/eingebettete-systeme/raspberry-pi-pico/ws2024.git
```

oder

```bash
# git clone https://code.fbi.h-da.de/eingebettete-systeme/raspberry-pi-pico/ws2024.git
git submodule update --init --recursive
```

Wenn ihr eures eigenes git Benutzten wollt müsst ihr den `.git` Ordner löschen.

```bash
rm -rf .git
git init
git add .
...
```

# Entwicklungsumgebung
Im Rahmen des Praktikums kommen folgende Entwicklungstools zum Einsatz:
 
```
code (Visual Studio Code)
cmake
openocd
minicom
arm-none-eabi-binutils
arm-none-eabi-gcc
arm-none-eabi-gdb
arm-none-eabi-newlib
```

Für VSCode brauchen wir folgende Erweiterung:
- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [Hex Editor](https://marketplace.visualstudio.com/items?itemName=ms-vscode.hexeditor)
- [Embedded Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.vscode-embedded-tools)
- [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
- [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

### Anleitungen
- [Virtual Box](#virtual-box)
- [Windows WSL](#windows-wsl)
- [macOS](#macos)
- [Ubuntu](#ubuntu)
- [Arch Linux](#archlinux)
- [Visual Studio Code & FAQ](#visual-studio-code-faq)

## Virtual Box
Ein aktualisiertes OVA Linux-Image für VirtualBox ist auf der [Labor-Webseite](https://fbi.h-da.de/studium/labore/mikroprozessor) verfügbar. Dieses Image enthält bereits alle erforderlichen Entwicklungstools vorinstalliert. Bitte beachten Sie, dass beim Einsatz von VirtualBox die notwendigen USB-Geräte entsprechend durchgereicht werden müssen.

## Windows WSL
Eine detaillierte Installationsanleitung für WSL finden Sie auf der [offiziellen Microsoft-Webseite](https://learn.microsoft.com/de-de/windows/wsl/install). Nach einigen Tests hat sich herausgestellt das Debian als beste Variante am wenigsten Probleme verursacht. Um die Installation zu starten, führen Sie den folgenden Befehl in PowerShell aus:

```powershell
wsl --install -d Debian
```

Um die benötigte Software (Toolchain, OpenOCD, etc.) zu installieren muss folgendes Kommando in einer Debian Shell ausgeführt werden:

```bash
sudo apt update && sudo apt upgrade -y && sudo apt install -y git cmake make curl wget usbutils build-essential openocd gdb-multiarch minicom python3 *-arm-none-eabi*
```

Um USB-Geräte innerhalb von WSL verwenden zu können, nutzen wir USB/IP. Eine ausführliche Installationsanleitung dazu finden Sie [hier](https://learn.microsoft.com/de-de/windows/wsl/connect-usb). Installieren Sie USB/IP **unter Windows** mit dem folgenden Befehl in einer administrativen PowerShell:

```powershell
winget install --exact dorssel.usbipd-win
```

Sollte [WinGet](https://github.com/microsoft/winget-cli) nicht als Befhl erkannt werden stellen Sie sicher das Sie die neuste Version des [App Installers](https://apps.microsoft.com/detail/9nblggh4nns1?hl=en-us&gl=DE) installiert haben. (Dem Link in den Microsoft Store folgen und evtl. updaten).

Die angeschlossenen USB-Geräte können mit dem folgenden Befehl in PowerShell angezeigt werden:

```powershell
usbipd list
```

Die Ausgabe des Befehls sollte in etwa folgendermaßen aussehen:

```
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
10-1   2e8a:000c  CMSIS-DAP v2 Interface, USB Serial Device (COM4)              Not shared

Persisted:
GUID                                  DEVICE
```

Bevor ein USB-Gerät an WSL angebunden werden kann, muss das Gerät für die Freigabe mittels des Befehls `usbipd bind` vorbereitet werden. Dieser Schritt erfordert Administratorrechte. Nach der Bindung sollte der Status des Geräts auf "Shared" wechseln. Verwenden Sie den folgenden Befehl in PowerShell mit Administratorrechten, um das Gerät freizugeben:

```powershell
usbipd bind -i 2e8a:000c
```

Das USB-Gerät kann mit dem folgenden Befehl an WSL angebunden werden. Dieser Schritt muss nach jedem Neustart des Computers wiederholt werden. Eine Automatisierung ist durch den Einsatz des Aufgabenplaners (`taskschd.msc`) möglich:

```powershell
usbipd attach -awi 2e8a:000c
```

Anschließend können Sie in WSL übeprüfen ob die Verbindung erfolgreich war mit:

```bash
lsusb
```

<s>Für die Installation der Tools innerhalb von WSL, beziehen Sie sich bitte auf die [Ubuntu-Installationsanleitung](#ubuntu).</s>

Für die Entwicklung mit [Visual Studio Code](https://code.visualstudio.com/) verwenden wir die [WSL-Erweiterung](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-wsl), die eine nahtlose Integration von VSCode mit dem Windows Subsystem for Linux ermöglicht. Eine detaillierte Anleitung zur Einrichtung und Nutzung finden Sie [hier](https://code.visualstudio.com/docs/remote/wsl).

In der Konfigurationsdatei `.vscode/launch.json` muss der `miDebuggerPath` auf `gdb-multiarch` angepasst werden, um den entsprechenden Debugger für die Entwicklung zu spezifizieren. Der angepasste Teil der Konfiguration sollte wie folgt aussehen:

```json
{
    "configurations": [
        {
            "miDebuggerPath": "gdb-multiarch",
        }
    ],
}
```

Fahren Sie anschließend mit der [Anleitung für VS Code](#step-by-step-vs-code-anleitung) fort

> Diese Anweisungen wurden getestet mit Debian 12 (WSL2) auf frischen Windows Installationen der Versionen: 10 (22H2 19045.2965), 10 (22H2 19045.5011) und 11 (24H2 26100.2033)

## macOS

Um die Toolchain unter macOS zu installieren folgen Sie bitte folgenden Anweisungen:

1. Installieren Sie den [Homebrew Paketmanager](https://brew.sh/)  
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

> Hinweis: In Homebrew gibt es zwei Pakete für unsere Toolchain wobei das Paket [**arm-none-eabi-gcc**](https://formulae.brew.sh/formula/arm-none-eabi-gcc#default) leider nicht mit unserer Konfiguration out-of-the-box funktioniert. Das korrekte Paket für uns heißt [**gcc-arm-embedded**](https://formulae.brew.sh/cask/gcc-arm-embedded#default)

> Hinweis: Sollten Sie bereits die inkorrekte Toolchain installiert haben (arm-none-eabi-*) deinstallieren Sie diese bitte mit ``brew remove arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-binutils``

2. Installieren Sie die korrete Version der Toolchain

```zsh
brew -v install --cask gcc-arm-embedded
```

3. Fahren Sie mit der [Anleitung für VS Code](#step-by-step-vs-code-anleitung) fort

## Ubuntu

```bash
sudo apt install make openocd minicom binutils-arm-none-eabi gcc-arm-none-eabi gdb-multiarch libnewlib-arm-none-eabi
```

Fahren Sie mit der [Anleitung für VS Code](#step-by-step-vs-code-anleitung) fort

## Archlinux
btw

```bash
pacman -S make openocd minicom arm-none-eabi-binutils arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-newlib
```

Fahren Sie mit der [Anleitung für VS Code](#step-by-step-vs-code-anleitung) fort

# Visual Studio Code FAQ

## Step by Step VS Code Anleitung

1. VisualStudio Code [hier](https://code.visualstudio.com/Download) laden & installieren.

2. (optional, für WSL Integration benötigt) [WSL-Erweiterung](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-wsl) installieren **nachdem** WSL installiert ist.

3. Mit einem Terminal in das geklonte Repository gehen (`cd /path/to/repo`)

4. VS Code mit `code .` im aktuellen Verzeichniss öffnen

5. Dem Workspace vertrauen (Yes, I trust the authors)

6. Die Recommended Extentions installieren (falls das Pop-up hierzu nicht auftaucht kann im Erweiterungen Menü einfach nach `@recommended` gesucht werden)
    - C/C++
    - CMake
    - CMake Tools
    - Hex Editor
    - Embedded Tools

7. `Ctrl+Shift+P` für die Command Eingabe, und `>Cmake Configure` eingeben.

8. Im anschließenden Pop-up die korrekte Toolchain auswählen (GCC 12.x.x **arm-none-eabi**)
    - Abgeschlossen ist die Konfiguration wenn `[cmake] -- Build files have been written to: /home/.../build` im log erscheint
    - Die erfolgreiche Konfiguration kann mit einem Build getestet werden (`Ctrl+Shift+B`) der erfolgreich mit `[100%] Built target main` enden sollte.

9. Falls Tasks wie `build` nicht gefunden werden sollten kann ein Reload mit `Ctrl+Shift+P` und `>Reload Window` ausgeführt werden.

10. Falls an irgend einer Stelle Probleme auftreten (wie z.B. eine falsche Toolchain wurde ausgewählt) kann diese Anleitung neugestartet werden mit  `Ctrl+Shift+P` und `>Reset Cmake Tools`. Anschließend muss wirder an Punkt 6 angefangen werden.

## Probleme 

Q: Unable to start debugging. ... Please specify the MIDebuggerPath option  
A: Im Pop-up `open launch.json` und `miDebuggerPath` auf `gdb-multiarch`setzen (siehe [WSL](#windows-wsl))

Q: Unable to start debugging. ... localhost:3333: Connection timed out.  
A: OpenOCD starten nachdem das Board verbunden ist: `Terminal` > `Run Task` > `openocd`

Q: OpenOCD Log sagt: `Error: unable to find a matching CMSIS-DAP device`  
A: Das Board ist nicht korrekt verbunden (VM USB / USBIP Einstellungen prüfen)

Q: OpenOCD Log sagt: `couldn't bind tcl to socket on port xxxx: Address already in use`  
A: Es läuft bereits eine OpenOCD Instanz, diese kann mit `pkill openocd` oder `killall openocd` oder mit einem Systemneustart beenden.