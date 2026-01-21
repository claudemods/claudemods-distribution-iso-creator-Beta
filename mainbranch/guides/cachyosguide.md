# 🚀 Claudemods CachyOS ISO Creator

C++ tool for building custom CachyOS Linux distribution ISOs. 🛠️

## 📋 Menu Options

- 📁 **Installation Path**: current directory as claudemods-distro
- 👤 **Set Username**
- 🔑 **Set Root Password**  
- 🔒 **Set User Password**
- 🌍 **Set Timezone**
- ⌨️ **Set Keyboard Layout**
- 🐧 **Set Kernel**
- 🖥️ **Select Desktop Environment**
- 📦 **Install Extra Packages**
- 🚀 **Start Installation**
- 🚪 **Exit**

## ❓ What It Does
🎯 **Creates bootable CachyOS ISO files** with your customized configuration selections.  
🔒 **All settings display exactly as entered** - including passwords for full transparency.  
💾 **Automatically saves configuration** to `configurationcachyos.txt` in the current directory for future reference and backup.

## 🖥️ Desktop Environments

- ⚫ **CachyOS TTY Grub** (Terminal Only)
- 🔵 **CachyOS KDE Grub**
- 🟣 **CachyOS GNOME Grub**

## 🐧 Kernel Options

- 🟢 **linux-cachyos** (Default)
- 🔵 **linux-cachyos-bore**
- 🟡 **linux-cachyos-eevdf**
- 🟠 **linux-cachyos-lts**
- 🔴 **linux-cachyos-rt-bore**
- 🛡️ **linux-cachyos-hardened**
- ⚙️ **linux-cachyos-server**
- 🎮 **linux-cachyos-deckify**
- 🧪 **linux-cachyos-rc**
- 🔧 **linux-cachyos-bmq**

  ## 🌍 Supported Timezones

- 🇺🇸 America/New_York (US English)
- 🇬🇧 Europe/London (UK English)
- 🇩🇪 Europe/Berlin (German)
- 🇫🇷 Europe/Paris (French)
- 🇪🇸 Europe/Madrid (Spanish)
- 🇮🇹 Europe/Rome (Italian)
- 🇯🇵 Asia/Tokyo (Japanese)
- 🌐 Custom timezone entry

## ⌨️ Keyboard Layouts

- 🇺🇸 us (US English)
- 🇬🇧 uk (UK English)
- 🇩🇪 de (German)
- 🇫🇷 fr (French)
- 🇪🇸 es (Spanish)
- 🇮🇹 it (Italian)
- 🇯🇵 jp (Japanese)
- 🌐 Custom layout entry

## ⚙️ Setup Calamares
🔧 Once you boot your ISO if you didn't select linux-cachyos kernel, the kernel you select will need to be added to `/usr/share/calamares/modules/initcpio.conf` and `/etc/calamares/modules/initcpio.conf`

📝 Change line `linux.cachyos` to e.g `linux-cachyos-bore` before opening calamares

## 🎯 Usage

1. ▶️ **Run the program**
2. ⚙️ **Configure all menu options**
3. 🚀 **Select "Start Installation"**
4. 📁 **Program creates ISO in current directory**

## 🛠️ Features

- 🎨 **Colorful terminal interface** with intuitive arrow-key navigation
- 💾 **Configuration persistence** - saves and loads your settings automatically
- 🔧 **Automatic resource extraction** - extracts required files on first run
- 📦 **Custom package selection** - add extra packages to your ISO
- 🖥️ **Multiple display managers** - GDM, SDDM, LightDM support
- 🛠️ **Calamares integration** - includes graphical installer with custom branding

  ## ⚠️ Important Notes

- 🔒 **Passwords are displayed in plaintext** for verification
- 💻 **Requires sudo privileges** for system operations
- 🌐 **Needs internet connection** for package downloads
- 💾 **Approximately 10-15GB disk space** required depending on what you choose
- ⏱️ **Process takes 15-60 minutes** depending on selection and internet speed

