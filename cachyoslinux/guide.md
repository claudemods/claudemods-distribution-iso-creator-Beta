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

## 💡 Kernel Information
ℹ️ Please note: the kernel you select you will need to be currently running for this to work

## ⚙️ Setup Calamares
🔧 Once you boot your ISO if you didn't select linux-cachyos kernel, the kernel you select will need to be added to `/usr/share/calamares/modules/initcpio.conf` and `/etc/calamares/modules/initcpio.conf`

📝 Change line `linux.cachyos` to e.g `linux-cachyos-bore` before opening calamares

## 🎯 Usage

1. ▶️ **Run the program**
2. ⚙️ **Configure all menu options**
3. 🚀 **Select "Start Installation"**
4. 📁 **Program creates ISO in current directory**
