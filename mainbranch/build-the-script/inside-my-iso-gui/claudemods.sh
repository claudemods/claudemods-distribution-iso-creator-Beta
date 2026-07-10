#!/bin/bash

# ============================================================
# Claudemods Distribution ISO Creator - Full Zenity GUI
# Direct conversion from C++ - all original commands preserved
# ============================================================

set -euo pipefail

CONFIG_FILE="$(pwd)/configurationclaudemods.txt"
CURRENT_DIR="$(pwd)"
TARGET_FOLDER="/mnt"
CALAMARES_FOLDER="$CURRENT_DIR/calamares-claudemods"

NEW_USERNAME=""
ROOT_PASSWORD=""
USER_PASSWORD=""
TIMEZONE=""
KEYBOARD_LAYOUT=""
CURRENT_DISTRO_NAME=""
EXTRA_PACKAGES=""
TARGET_DRIVE=""
FILESYSTEM_TYPE=""

# -----------------------------------------------------------
# Save Configuration (exactly as C++)
# -----------------------------------------------------------
save_configuration() {
    local configFile="$CONFIG_FILE"
    cat > "$configFile" <<EOF
username=$NEW_USERNAME
root_password=$ROOT_PASSWORD
user_password=$USER_PASSWORD
timezone=$TIMEZONE
keyboard_layout=$KEYBOARD_LAYOUT
current_distro=$CURRENT_DISTRO_NAME
extra_packages=$EXTRA_PACKAGES
target_drive=$TARGET_DRIVE
filesystem_type=$FILESYSTEM_TYPE
EOF
    echo "Configuration saved to $configFile"
}

# -----------------------------------------------------------
# Load Configuration (exactly as C++)
# -----------------------------------------------------------
load_configuration() {
    local configFile="$CONFIG_FILE"
    if [[ -f "$configFile" ]]; then
        while IFS='=' read -r key value; do
            case "$key" in
                username) NEW_USERNAME="$value" ;;
                root_password) ROOT_PASSWORD="$value" ;;
                user_password) USER_PASSWORD="$value" ;;
                timezone) TIMEZONE="$value" ;;
                keyboard_layout) KEYBOARD_LAYOUT="$value" ;;
                current_distro) CURRENT_DISTRO_NAME="$value" ;;
                extra_packages) EXTRA_PACKAGES="$value" ;;
                target_drive) TARGET_DRIVE="$value" ;;
                filesystem_type) FILESYSTEM_TYPE="$value" ;;
            esac
        done < "$configFile"
        echo "Configuration loaded from $configFile"
    else
        echo "No existing configuration found. Starting with default settings."
    fi
}

# -----------------------------------------------------------
# Execute command helper
# -----------------------------------------------------------
execute_command() {
    eval "$1" 2>/dev/null || true
}

# -----------------------------------------------------------
# Extract Required Files (exactly as C++)
# -----------------------------------------------------------
extract_required_files() {
    local currentDir="$CURRENT_DIR"
    local calamaresFolder="$CALAMARES_FOLDER"
    
    echo "Checking for required folders..."
    
    local buildImageExists=false
    local calamaresFilesExists=false
    local workingHooksExists=false
    
    [[ -d "$calamaresFolder/build-image-arch-img" ]] && buildImageExists=true
    [[ -d "$calamaresFolder/calamares-files" ]] && calamaresFilesExists=true
    [[ -d "$calamaresFolder/working-hooks-btrfs-ext4" ]] && workingHooksExists=true
    
    if $buildImageExists && $calamaresFilesExists && $workingHooksExists; then
        echo "All required folders already exist."
        return 0
    fi
    
    if [[ ! -d "$calamaresFolder" ]]; then
        echo "Creating calamares-claudemods folder..."
        execute_command "sudo mkdir -p $calamaresFolder"
    fi
    
    local sourceDir="$currentDir/calamares-per-distro/claudemods"
    local zipFiles=(
        "calamares-claudemods.zip"
        "claudemods.zip"
        "build-image-claudemods.zip"
    )
    
    for zipFile in "${zipFiles[@]}"; do
        local sourcePath="$sourceDir/$zipFile"
        if [[ -f "$sourcePath" ]]; then
            echo "Extracting $zipFile..."
            execute_command "sudo unzip -q $sourcePath -d $calamaresFolder >/dev/null 2>&1"
            echo "Done."
        fi
    done
    
    echo "Extraction process completed."
    return 0
}

# -----------------------------------------------------------
# Setup Target Directory (exactly as C++)
# -----------------------------------------------------------
setup_target_directory() {
    local target_folder="$1"
    local currentDir="$CURRENT_DIR"
    
    echo "Creating target directory: $target_folder"
    execute_command "sudo mkdir -p $target_folder"
    
    if [[ ! -d "$target_folder" ]]; then
        echo "Failed to create target directory: $target_folder"
        return 1
    fi
    
    echo "Target directory created successfully!"
    
    execute_command "sudo mkdir -p $target_folder/usr/"
    execute_command "sudo mkdir -p $target_folder/usr/lib"
    execute_command "sudo mkdir -p $target_folder/usr/lib/initcpio/"
    execute_command "sudo mkdir -p $target_folder/usr/lib/initcpio/udev/"
    execute_command "sudo mkdir -p $target_folder/etc/sysctl.d"
    execute_command "sudo cp -r $currentDir/needed-files/11-dm-initramfs.rules $target_folder/usr/lib/initcpio/udev/11-dm-initramfs.rules"
    execute_command "sudo cp -r $currentDir/needed-files/11-dm-initramfs.rules /usr/lib/initcpio/udev/11-dm-initramfs.rules"
    execute_command "sudo cp -r $currentDir/needed-files/80-gamecompatibility.conf $target_folder/etc/sysctl.d/80-gamecompatibility.conf"
    execute_command "sudo mkdir -p $target_folder/etc/pacman.d"
    execute_command "sudo mkdir -p $target_folder/boot/grub"
    execute_command "sudo mkdir -p $target_folder/usr/share/grub/themes"
    execute_command "sudo mkdir -p $target_folder/usr/share/plymouth/themes"
    execute_command "sudo mkdir -p $target_folder/usr/local/bin"
    execute_command "sudo mkdir -p $target_folder/etc/systemd/system"
    execute_command "sudo mkdir -p $target_folder/etc/sddm.conf.d"
    return 0
}

# -----------------------------------------------------------
# Setup Pacman and Files (exactly as C++)
# -----------------------------------------------------------
setup_pacman_and_files() {
    local target_folder="$1"
    local currentDir="$CURRENT_DIR"
    
    execute_command "sudo cp -r $currentDir/needed-files/vconsole.conf $target_folder/etc/vconsole.conf"
    execute_command "sudo cp -r /etc/resolv.conf $target_folder/etc/resolv.conf"
    execute_command "sudo unzip -o $currentDir/needed-files/pacman.d.zip -d $target_folder/etc/pacman.d"
    execute_command "sudo unzip -o $currentDir/needed-files/pacman.d.zip -d /etc/pacman.d"
    execute_command "sudo cp -r $currentDir/needed-files/pacman.conf $target_folder/etc/pacman.conf"
    execute_command "sudo cp -r $currentDir/needed-files/pacman.conf /etc/pacman.conf"
    execute_command "sudo pacman -Sy"
    execute_command "sudo pacman -S archlinux-keyring"
    execute_command "sudo pacman-key --populate"
    execute_command "sudo pacman-key --init"
}

# -----------------------------------------------------------
# Verify Pacstrap Success (exactly as C++)
# -----------------------------------------------------------
verify_pacstrap_success() {
    local target_folder="$1"
    local test_bin="$target_folder/bin/bash"
    
    if [[ ! -f "$test_bin" ]]; then
        echo "pacstrap failed! /bin/bash not found in target."
        return 1
    fi
    return 0
}

# -----------------------------------------------------------
# Setup Boot Directory (exactly as C++)
# -----------------------------------------------------------
setup_boot_directory() {
    local target_folder="$1"
    execute_command "sudo mkdir -p $target_folder/boot"
    execute_command "sudo mkdir -p $target_folder/boot/grub"
    execute_command "sudo touch $target_folder/boot/grub/grub.cfg.new"
}

# -----------------------------------------------------------
# Create User Home Structure (exactly as C++)
# -----------------------------------------------------------
create_user_home_structure() {
    local target_folder="$1"
    execute_command "sudo mkdir -p $target_folder/home/$NEW_USERNAME/.config/fish"
    execute_command "sudo mkdir -p $target_folder/home/$NEW_USERNAME/.local/share/konsole"
    execute_command "sudo mkdir -p $target_folder/home/$NEW_USERNAME/.local/share"
    execute_command "sudo chmod +x $target_folder/home/$NEW_USERNAME/.config/fish/config.fish"
    execute_command "sudo chroot $target_folder /bin/bash -c \"chmod +x /usr/share/fish/config.fish\""
}

# -----------------------------------------------------------
# Fix User Places XBEL (exactly as C++)
# -----------------------------------------------------------
fix_user_places_xbel() {
    local target_folder="$1"
    local home_folder
    home_folder=$(sudo ls -1 "$target_folder/home" | grep -v '^\.' | head -1)
    
    if [[ -n "$home_folder" ]]; then
        local user_places_file="$target_folder/home/$home_folder/.local/share/user-places.xbel"
        execute_command "sudo sed -i 's/spitfire/$home_folder/g' $user_places_file"
    fi
}

fix_user_places_xbel_apex() {
    local target_folder="$1"
    local home_folder
    home_folder=$(sudo ls -1 "$target_folder/home" | grep -v '^\.' | head -1)
    
    if [[ -n "$home_folder" ]]; then
        local user_places_file="$target_folder/home/$home_folder/.local/share/user-places.xbel"
        execute_command "sudo sed -i 's/apex/$home_folder/g' $user_places_file"
    fi
}

# -----------------------------------------------------------
# Install Calamares (exactly as C++)
# -----------------------------------------------------------
install_calamares() {
    local target_folder="$1"
    local currentDir="$CURRENT_DIR"
    
    echo "Installing Calamares installer and setting up iso ..."
    
    execute_command "sudo cp $currentDir/needed-files/kwalletrc $target_folder/home/$NEW_USERNAME/.config/kwalletrc"
    execute_command "sudo rm -rf $target_folder/home/$NEW_USERNAME/.local/share/kwalletd/*"
    execute_command "sudo cp -r $currentDir/needed-files/wireless-regdom $target_folder/etc/conf.d/wireless-regdom"
    execute_command "setfattr -n user.kde.fm.viewproperties#1 -v '[Dolphin]\\012Timestamp=2026,1,20,17,27,36.341\\012Version=4\\012\\012[Settings]\\012HiddenFilesShown=true' $target_folder/home/$NEW_USERNAME/.local/share/dolphin/view_properties/global"
    
    echo "installation completed!"
}

# -----------------------------------------------------------
# Setup Bootloader and Drive (exactly as C++)
# -----------------------------------------------------------
setup_bootloader_and_drive() {
    clear
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║ Setup Bootloader and Drive                                  ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo ""
    echo "Current Drive Settings:"
    echo "Target Drive: ${TARGET_DRIVE:-[Not Set]}"
    echo "Filesystem: ${FILESYSTEM_TYPE:-[Not Set]}"
    echo ""
    echo "Available drives:"
    echo "NAME        SIZE    MODEL"
    echo "----------------------------------------"
    
    lsblk -d -o NAME,SIZE,MODEL 2>/dev/null | grep -v 'loop\|sr0\|zram' | while read -r name size model; do
        printf "/dev/%-10s %-8s %s\n" "$name" "$size" "$model"
    done
    
    echo ""
    echo "WARNING: The selected drive will be COMPLETELY ERASED!"
    
    # Zenity drive selection
    local drive_list=""
    while IFS= read -r line; do
        [[ -z "$line" ]] && continue
        local name=$(echo "$line" | awk '{print $1}')
        local size=$(echo "$line" | awk '{print $2}')
        local model=$(echo "$line" | cut -d' ' -f3-)
        [[ -b "/dev/$name" ]] && drive_list+="/dev/$name $size $model\n"
    done < <(lsblk -dno NAME,SIZE,MODEL 2>/dev/null | grep -v 'loop\|sr0\|zram')
    
    local drive_input
    drive_input=$(zenity --list --title="Setup Bootloader and Drive" \
        --text="Available drives:\n\nWARNING: The selected drive will be COMPLETELY ERASED!\n\nSelect target drive:" \
        --column="Device" --column="Size" --column="Model" \
        --print-column=1 \
        $(echo -e "$drive_list") \
        --width=600 --height=400 \
        --extra-button="Manual Entry" 2>/dev/null)
    
    if [[ "$drive_input" == "Manual Entry" ]]; then
        drive_input=$(zenity --entry --title="Target Drive" --text="Enter target drive (e.g., /dev/sda):" --entry-text="$TARGET_DRIVE" --width=400 2>/dev/null)
    fi
    
    if [[ -z "$drive_input" ]]; then
        echo "No drive selected. Keeping current setting."
    elif [[ ! -b "$drive_input" ]]; then
        echo "Error: $drive_input is not a valid block device!"
    else
        TARGET_DRIVE="$drive_input"
        echo "Target drive set to: $TARGET_DRIVE"
    fi
    
    # Filesystem selection
    if [[ -n "$TARGET_DRIVE" ]]; then
        local fs_choice
        fs_choice=$(zenity --list --title="Select Filesystem Type" \
            --text="Select filesystem type:" \
            --column="Filesystem" \
            "Btrfs (with subvolumes, compression, snapshots support)" \
            "Ext4 (standard filesystem)" \
            --width=600 --height=200 2>/dev/null)
        
        if [[ "$fs_choice" == "Btrfs (with subvolumes, compression, snapshots support)" ]]; then
            FILESYSTEM_TYPE="btrfs"
            echo "Filesystem set to: Btrfs"
            echo "Btrfs subvolumes will be created: @, @home, @root, @srv, @cache, @tmp, @log"
            echo "Compression: zstd level 22"
        else
            FILESYSTEM_TYPE="ext4"
            echo "Filesystem set to: Ext4"
        fi
    fi
    
    # Show summary
    if [[ -n "$TARGET_DRIVE" && -n "$FILESYSTEM_TYPE" ]]; then
        local fs_label
        [[ "$FILESYSTEM_TYPE" == "btrfs" ]] && fs_label="Btrfs" || fs_label="Ext4"
        
        echo ""
        echo "═══════════════════════════════════════"
        echo "  Drive Configuration Summary:"
        echo "  Drive: $TARGET_DRIVE"
        echo "  Partition 1: ${TARGET_DRIVE}1 (EFI - FAT32, 550MB)"
        echo "  Partition 2: ${TARGET_DRIVE}2 (Root - $fs_label)"
        echo "  Bootloader: GRUB (UEFI)"
        echo "═══════════════════════════════════════"
        echo ""
        echo "WARNING: ALL DATA ON $TARGET_DRIVE WILL BE DESTROYED!"
        
        zenity --warning --title="Drive Configuration Summary" \
            --text="Drive Configuration Summary:\n\n  Drive: $TARGET_DRIVE\n  Partition 1: ${TARGET_DRIVE}1 (EFI - FAT32, 550MB)\n  Partition 2: ${TARGET_DRIVE}2 (Root - $fs_label)\n  Bootloader: GRUB (UEFI)\n\nWARNING: ALL DATA ON $TARGET_DRIVE WILL BE DESTROYED!" \
            --width=500 2>/dev/null
    fi
    
    save_configuration
}

# -----------------------------------------------------------
# Prepare Target Partitions (exactly as C++)
# -----------------------------------------------------------
prepare_target_partitions() {
    echo "Preparing target partitions on $TARGET_DRIVE..."
    
    execute_command "sudo umount -f ${TARGET_DRIVE}* 2>/dev/null || true"
    execute_command "sudo wipefs -a $TARGET_DRIVE"
    execute_command "sudo parted -s $TARGET_DRIVE mklabel gpt"
    execute_command "sudo parted -s $TARGET_DRIVE mkpart primary fat32 1MiB 551MiB"
    execute_command "sudo parted -s $TARGET_DRIVE mkpart primary $FILESYSTEM_TYPE 551MiB 100%"
    execute_command "sudo parted -s $TARGET_DRIVE set 1 esp on"
    execute_command "sudo partprobe $TARGET_DRIVE"
    sleep 2
    
    local efi_part="${TARGET_DRIVE}1"
    local root_part="${TARGET_DRIVE}2"
    
    if [[ ! -b "$efi_part" || ! -b "$root_part" ]]; then
        echo "Error: Failed to create partitions"
        exit 1
    fi
    
    execute_command "sudo mkfs.vfat -F32 $efi_part"
    
    if [[ "$FILESYSTEM_TYPE" == "btrfs" ]]; then
        execute_command "sudo mkfs.btrfs -f -L ROOT $root_part"
    else
        execute_command "sudo mkfs.ext4 -F -L ROOT $root_part"
    fi
}

# -----------------------------------------------------------
# Setup Btrfs Subvolumes (exactly as C++)
# -----------------------------------------------------------
setup_btrfs_subvolumes() {
    local root_part="${TARGET_DRIVE}2"
    
    echo "Setting up Btrfs subvolumes with zstd:22 compression..."
    
    execute_command "sudo mount $root_part /mnt"
    execute_command "sudo btrfs subvolume create /mnt/@"
    execute_command "sudo btrfs subvolume create /mnt/@home"
    execute_command "sudo btrfs subvolume create /mnt/@root"
    execute_command "sudo btrfs subvolume create /mnt/@srv"
    execute_command "sudo btrfs subvolume create /mnt/@cache"
    execute_command "sudo btrfs subvolume create /mnt/@tmp"
    execute_command "sudo btrfs subvolume create /mnt/@log"
    execute_command "sudo mkdir -p /mnt/@/var/lib"
    execute_command "sudo btrfs subvolume create /mnt/@/var/lib/portables"
    execute_command "sudo btrfs subvolume create /mnt/@/var/lib/machines"
    execute_command "sudo umount /mnt"
    execute_command "sudo mount -o subvol=@,compress=zstd:22,compress-force=zstd:22 $root_part /mnt"
    execute_command "sudo mkdir -p /mnt/{home,root,srv,tmp,var/{cache,log},var/lib/{portables,machines},boot/efi}"
    execute_command "sudo mount -o subvol=@home,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/home"
    execute_command "sudo mount -o subvol=@root,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/root"
    execute_command "sudo mount -o subvol=@srv,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/srv"
    execute_command "sudo mount -o subvol=@cache,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/var/cache"
    execute_command "sudo mount -o subvol=@tmp,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/tmp"
    execute_command "sudo mount -o subvol=@log,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/var/log"
    execute_command "sudo mount -o subvol=@/var/lib/portables,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/var/lib/portables"
    execute_command "sudo mount -o subvol=@/var/lib/machines,compress=zstd:22,compress-force=zstd:22 $root_part /mnt/var/lib/machines"
}

# -----------------------------------------------------------
# Setup Ext4 Filesystem (exactly as C++)
# -----------------------------------------------------------
setup_ext4_filesystem() {
    local root_part="${TARGET_DRIVE}2"
    echo "Setting up Ext4 filesystem..."
    execute_command "sudo mount $root_part /mnt"
    execute_command "sudo mkdir -p /mnt/{home,boot/efi,etc,usr,var,proc,sys,dev,tmp,run}"
}

# -----------------------------------------------------------
# Mount/Unmount System Dirs (exactly as C++)
# -----------------------------------------------------------
mount_system_dirs() {
    execute_command "sudo mkdir -p /mnt/dev"
    execute_command "sudo mkdir -p /mnt/dev/pts"
    execute_command "sudo mkdir -p /mnt/proc"
    execute_command "sudo mkdir -p /mnt/sys"
    execute_command "sudo mkdir -p /mnt/run"
    execute_command "sudo mkdir -p /mnt/etc"
    execute_command "sudo mount --bind /dev /mnt/dev"
    execute_command "sudo mount --bind /dev/pts /mnt/dev/pts"
    execute_command "sudo mount --bind /proc /mnt/proc"
    execute_command "sudo mount --bind /sys /mnt/sys"
    execute_command "sudo mount --bind /run /mnt/run"
}

unmount_system_dirs() {
    execute_command "sudo umount /mnt/dev/pts"
    execute_command "sudo umount /mnt/dev"
    execute_command "sudo umount /mnt/proc"
    execute_command "sudo umount /mnt/sys"
    execute_command "sudo umount /mnt/run"
}

unmount_target() {
    echo "Unmounting target filesystems..."
    execute_command "sudo umount -l /mnt 2>/dev/null || true"
}

# -----------------------------------------------------------
# Create User (exactly as C++)
# -----------------------------------------------------------
create_user() {
    execute_command "sudo chroot /mnt /bin/bash -c \"useradd -m -G wheel -s /bin/bash $NEW_USERNAME\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'root:$ROOT_PASSWORD' | chpasswd\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo '$NEW_USERNAME:$USER_PASSWORD' | chpasswd\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\""
}

# -----------------------------------------------------------
# Apply Timezone/Keyboard Settings (exactly as C++)
# -----------------------------------------------------------
apply_timezone_keyboard_settings() {
    echo "Setting timezone to: $TIMEZONE"
    execute_command "sudo chroot /mnt /bin/bash -c \"ln -sf /usr/share/zoneinfo/$TIMEZONE /etc/localtime\""
    execute_command "sudo chroot /mnt /bin/bash -c \"hwclock --systohc\""
    
    echo "Setting keyboard layout to: $KEYBOARD_LAYOUT"
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'KEYMAP=$KEYBOARD_LAYOUT' > /etc/vconsole.conf\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\""
    execute_command "sudo chroot /mnt /bin/bash -c \"locale-gen\""
}

# -----------------------------------------------------------
# Install GRUB (exactly as C++)
# -----------------------------------------------------------
install_grub() {
    echo "Installing GRUB bootloader..."
    local efi_part="${TARGET_DRIVE}1"
    
    execute_command "sudo mount $efi_part /mnt/boot/efi"
    execute_command "sudo mount --bind /sys/firmware/efi/efivars /mnt/sys/firmware/efi/efivars 2>/dev/null || true"
    execute_command "sudo mount --bind /dev /mnt/dev"
    execute_command "sudo mount --bind /dev/pts /mnt/dev/pts"
    execute_command "sudo mount --bind /proc /mnt/proc"
    execute_command "sudo mount --bind /sys /mnt/sys"
    execute_command "sudo mount --bind /run /mnt/run"
    
    if [[ "$FILESYSTEM_TYPE" == "btrfs" ]]; then
        execute_command "sudo touch /mnt/etc/fstab"
        execute_command "sudo cp btrfsfstabcompressed.sh /mnt/opt/btrfsfstabcompressed.sh"
        execute_command "sudo chroot /mnt /bin/bash -c \"modprobe efivarfs 2>/dev/null || true; mount -t efivarfs efivarfs /sys/firmware/efi/efivars 2>/dev/null || true; grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=Cachyos --recheck; grub-mkconfig -o /boot/grub/grub.cfg; ./opt/btrfsfstabcompressed.sh 2>/dev/null; rm -rf /opt/btrfsfstabcompressed.sh 2>/dev/null; rm -rf /var/cache/pacman/pkg/*; mkinitcpio -P\""
    else
        execute_command "sudo chroot /mnt /bin/bash -c \"modprobe efivarfs 2>/dev/null || true; mount -t efivarfs efivarfs /sys/firmware/efi/efivars 2>/dev/null || true; genfstab -U / >> /etc/fstab; grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=cachyos --recheck; grub-mkconfig -o /boot/grub/grub.cfg; rm -rf /var/cache/pacman/pkg/*; mkinitcpio -P\""
    fi
}

# -----------------------------------------------------------
# Post Install Menu (exactly as C++)
# -----------------------------------------------------------
post_install_menu() {
    local menu_running=true
    while $menu_running; do
        echo ""
        echo "╔══════════════════════════════════════════════╗"
        echo "║           INSTALLATION COMPLETE              ║"
        echo "╠══════════════════════════════════════════════╣"
        echo "║  1. Reboot now                              ║"
        echo "║  2. Exit to shell                           ║"
        echo "╚══════════════════════════════════════════════╝"
        echo ""
        
        local choice
        choice=$(zenity --list --title="INSTALLATION COMPLETE" \
            --text="INSTALLATION COMPLETE\n\nChoose an option:" \
            --column="Option" \
            "1. Reboot now" \
            "2. Exit to shell" \
            --width=400 --height=200 2>/dev/null)
        
        if [[ "$choice" == "1. Reboot now" ]]; then
            echo "Unmounting and rebooting..."
            unmount_target
            execute_command "sudo reboot"
            menu_running=false
        elif [[ "$choice" == "2. Exit to shell" ]]; then
            echo "Exiting to shell. System is still mounted at /mnt"
            menu_running=false
        else
            echo "Invalid option. Please choose 1 or 2."
        fi
    done
}

# -----------------------------------------------------------
# Check Settings Configured (exactly as C++)
# -----------------------------------------------------------
check_settings_configured() {
    if [[ -z "$TARGET_DRIVE" ]]; then
        echo "Error: Target drive not set! Use 'Setup Bootloader and Drive' first."
        return 1
    fi
    if [[ -z "$FILESYSTEM_TYPE" ]]; then
        echo "Error: Filesystem type not set! Use 'Setup Bootloader and Drive' first."
        return 1
    fi
    if [[ -z "$NEW_USERNAME" ]]; then
        echo "Error: Username not set!"
        return 1
    fi
    if [[ -z "$ROOT_PASSWORD" ]]; then
        echo "Error: Root password not set!"
        return 1
    fi
    if [[ -z "$USER_PASSWORD" ]]; then
        echo "Error: User password not set!"
        return 1
    fi
    if [[ -z "$TIMEZONE" ]]; then
        echo "Error: Timezone not set!"
        return 1
    fi
    if [[ -z "$KEYBOARD_LAYOUT" ]]; then
        echo "Error: Keyboard layout not set!"
        return 1
    fi
    if [[ -z "$CURRENT_DISTRO_NAME" ]]; then
        echo "Error: No distribution selected!"
        return 1
    fi
    return 0
}

# -----------------------------------------------------------
# DISTRO INSTALL FUNCTIONS (exactly as C++, all commands preserved)
# -----------------------------------------------------------

install_spitfire_ckge_minimal() {
    echo "Installing Spitfire CKGE Minimal to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    echo "Installing base system with pacstrap..."
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop calamares-fix protonup-qt hhd adjustor hhd-ui sddm piper"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/spitfire-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $currentDir/spitfire-minimal-v1.01.zip"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_spitfire_ckge_minimal_dev() {
    echo "Installing Spitfire CKGE Minimal Dev to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    echo "Installing base system with pacstrap..."
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-dev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/spitfire-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $currentDir/spitfire-minimal-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_spitfire_ckge_full() {
    echo "Installing Spitfire CKGE Full to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/spitfire-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $currentDir/spitfire-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_spitfire_ckge_full_dev() {
    echo "Installing Spitfire CKGE Full Dev to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/spitfire-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $currentDir/spitfire-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_spitfire_ckge_black_full() {
    echo "Installing Spitfire CKGE Black Full to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-black-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-CKGBE-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/spitfire-full-black-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo rm -rf $currentDir/spitfire-full-black-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_spitfire_ckge_black_full_dev() {
    echo "Installing Spitfire CKGE Black Full Dev to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-black-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-CKGBE-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/spitfire-full-black-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo rm -rf $currentDir/spitfire-full-black-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_apex_ckge_minimal() {
    echo "Installing Apex CKGE Minimal to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop calamares-fix protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/apex-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $currentDir/apex-minimal-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_apex_ckge_minimal_dev() {
    echo "Installing Apex CKGE Minimal Dev to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-dev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm piper"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/apex-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $currentDir/apex-minimal-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_apex_ckge_full() {
    echo "Installing Apex CKGE Full to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/apex-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $currentDir/apex-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

install_apex_ckge_full_dev() {
    echo "Installing Apex CKGE Full Dev to $TARGET_DRIVE..."
    local target_folder="/mnt"
    local currentDir="$CURRENT_DIR"
    
    if ! setup_target_directory "$target_folder"; then return; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $currentDir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $currentDir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $currentDir/apex-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $currentDir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $currentDir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $currentDir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $currentDir/apex-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    echo ""
    echo "Installation complete! System installed to $TARGET_DRIVE"
    post_install_menu
}

# -----------------------------------------------------------
# Start Installation (exactly as C++)
# -----------------------------------------------------------
start_installation() {
    if ! check_settings_configured; then
        echo "Cannot proceed with installation. Please configure all settings first."
        zenity --error --title="Configuration Error" --text="Cannot proceed with installation. Please configure all settings first." --width=400 2>/dev/null
        return
    fi
    
    prepare_target_partitions
    
    if [[ "$FILESYSTEM_TYPE" == "btrfs" ]]; then
        setup_btrfs_subvolumes
    else
        setup_ext4_filesystem
    fi
    
    case "$CURRENT_DISTRO_NAME" in
        "Spitfire-CKGE-Minimal") install_spitfire_ckge_minimal ;;
        "Spitfire-CKGE-Minimal-Dev") install_spitfire_ckge_minimal_dev ;;
        "Spitfire-CKGE-Full") install_spitfire_ckge_full ;;
        "Spitfire-CKGE-Full-Dev") install_spitfire_ckge_full_dev ;;
        "Spitfire-CKGE-Black-Full") install_spitfire_ckge_black_full ;;
        "Spitfire-CKGE-Black-Full-Dev") install_spitfire_ckge_black_full_dev ;;
        "Apex-CKGE-Minimal") install_apex_ckge_minimal ;;
        "Apex-CKGE-Minimal-Dev") install_apex_ckge_minimal_dev ;;
        "Apex-CKGE-Full") install_apex_ckge_full ;;
        "Apex-CKGE-Full-Dev") install_apex_ckge_full_dev ;;
        *) zenity --error --title="Error" --text="No valid distribution selected!" --width=400 2>/dev/null ;;
    esac
}

# -----------------------------------------------------------
# Zenity Input Functions (replacing C++ get_input)
# -----------------------------------------------------------
set_username() {
    NEW_USERNAME=$(zenity --entry --title="Username" --text="Enter username:" --entry-text="$NEW_USERNAME" --width=400 2>/dev/null)
    save_configuration
}

set_root_password() {
    ROOT_PASSWORD=$(zenity --password --title="Root Password" --text="Enter root password:" --width=400 2>/dev/null)
    save_configuration
}

set_user_password() {
    USER_PASSWORD=$(zenity --password --title="User Password" --text="Enter user password:" --width=400 2>/dev/null)
    save_configuration
}

set_timezone() {
    local choice
    choice=$(zenity --list --title="Select Timezone" \
        --column="Timezone" \
        "America/New_York" "Europe/London" "Europe/Berlin" "Europe/Paris" \
        "Europe/Madrid" "Europe/Rome" "Asia/Tokyo" "Other (manual entry)" \
        --width=400 --height=350 2>/dev/null)
    
    case "$choice" in
        "America/New_York") TIMEZONE="America/New_York" ;;
        "Europe/London") TIMEZONE="Europe/London" ;;
        "Europe/Berlin") TIMEZONE="Europe/Berlin" ;;
        "Europe/Paris") TIMEZONE="Europe/Paris" ;;
        "Europe/Madrid") TIMEZONE="Europe/Madrid" ;;
        "Europe/Rome") TIMEZONE="Europe/Rome" ;;
        "Asia/Tokyo") TIMEZONE="Asia/Tokyo" ;;
        "Other (manual entry)") TIMEZONE=$(zenity --entry --title="Timezone" --text="Enter timezone (e.g., Europe/Berlin):" --width=400 2>/dev/null) ;;
    esac
    save_configuration
}

set_keyboard_layout() {
    local choice
    choice=$(zenity --list --title="Select Keyboard Layout" \
        --column="Layout" \
        "us" "uk" "de" "fr" "es" "it" "jp" "Other (manual entry)" \
        --width=400 --height=350 2>/dev/null)
    
    case "$choice" in
        "us") KEYBOARD_LAYOUT="us" ;;
        "uk") KEYBOARD_LAYOUT="uk" ;;
        "de") KEYBOARD_LAYOUT="de" ;;
        "fr") KEYBOARD_LAYOUT="fr" ;;
        "es") KEYBOARD_LAYOUT="es" ;;
        "it") KEYBOARD_LAYOUT="it" ;;
        "jp") KEYBOARD_LAYOUT="jp" ;;
        "Other (manual entry)") KEYBOARD_LAYOUT=$(zenity --entry --title="Keyboard Layout" --text="Enter keyboard layout (e.g., br, ru, pt):" --width=400 2>/dev/null) ;;
    esac
    save_configuration
}

set_wireless_regdom() {
    local currentDir="$CURRENT_DIR"
    local wireless_regdom_file="$currentDir/needed-files/wireless-regdom"
    
    echo "Opening wireless regulatory domain file for editing..."
    echo "File: $wireless_regdom_file"
    echo "Use Ctrl+X to exit nano after editing"
    
    zenity --info --title="Wireless Regdom" --text="Opening wireless regulatory domain file for editing...\n\nFile: $wireless_regdom_file\n\nUse Ctrl+X to exit nano after editing" --width=500 2>/dev/null
    
    if command -v nano &>/dev/null; then
        x-terminal-emulator -e "nano $wireless_regdom_file" 2>/dev/null || nano "$wireless_regdom_file"
    fi
    
    echo "Wireless regulatory domain file updated."
}

set_extra_packages() {
    EXTRA_PACKAGES=$(zenity --entry --title="Extra Packages" --text="Enter extra packages (space separated):" --entry-text="$EXTRA_PACKAGES" --width=500 2>/dev/null)
    save_configuration
}

# -----------------------------------------------------------
# Distro Selection (exactly as C++)
# -----------------------------------------------------------
show_distro_selection() {
    local choice
    choice=$(zenity --list --title="Select Distribution to Install" \
        --column="Distribution" \
        "Install Spitfire CKGE Minimal" \
        "Install Spitfire CKGE Minimal Dev" \
        "Install Spitfire CKGE Full" \
        "Install Spitfire CKGE Full Dev" \
        "Install Spitfire CKGE Black Full" \
        "Install Spitfire CKGE Black Full Dev" \
        "Install Apex CKGE Minimal" \
        "Install Apex CKGE Minimal Dev" \
        "Install Apex CKGE Full" \
        "Install Apex CKGE Full Dev" \
        "Back to Main Menu" \
        --width=500 --height=400 2>/dev/null)
    
    case "$choice" in
        "Install Spitfire CKGE Minimal")
            CURRENT_DISTRO_NAME="Spitfire-CKGE-Minimal"
            save_configuration
            zenity --info --title="Selected" --text="Spitfire CKGE Minimal selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Spitfire CKGE Minimal Dev")
            CURRENT_DISTRO_NAME="Spitfire-CKGE-Minimal-Dev"
            save_configuration
            zenity --info --title="Selected" --text="Spitfire CKGE Minimal Dev selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Spitfire CKGE Full")
            CURRENT_DISTRO_NAME="Spitfire-CKGE-Full"
            save_configuration
            zenity --info --title="Selected" --text="Spitfire CKGE Full selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Spitfire CKGE Full Dev")
            CURRENT_DISTRO_NAME="Spitfire-CKGE-Full-Dev"
            save_configuration
            zenity --info --title="Selected" --text="Spitfire CKGE Full Dev selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Spitfire CKGE Black Full")
            CURRENT_DISTRO_NAME="Spitfire-CKGE-Black-Full"
            save_configuration
            zenity --info --title="Selected" --text="Spitfire CKGE Black Full selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Spitfire CKGE Black Full Dev")
            CURRENT_DISTRO_NAME="Spitfire-CKGE-Black-Full-Dev"
            save_configuration
            zenity --info --title="Selected" --text="Spitfire CKGE Black Full Dev selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Apex CKGE Minimal")
            CURRENT_DISTRO_NAME="Apex-CKGE-Minimal"
            save_configuration
            zenity --info --title="Selected" --text="Apex CKGE Minimal selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Apex CKGE Minimal Dev")
            CURRENT_DISTRO_NAME="Apex-CKGE-Minimal-Dev"
            save_configuration
            zenity --info --title="Selected" --text="Apex CKGE Minimal Dev selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Apex CKGE Full")
            CURRENT_DISTRO_NAME="Apex-CKGE-Full"
            save_configuration
            zenity --info --title="Selected" --text="Apex CKGE Full selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
        "Install Apex CKGE Full Dev")
            CURRENT_DISTRO_NAME="Apex-CKGE-Full-Dev"
            save_configuration
            zenity --info --title="Selected" --text="Apex CKGE Full Dev selected. Use 'Start Installation' to begin." --width=400 2>/dev/null
            ;;
    esac
}

# -----------------------------------------------------------
# Display Current Settings (exactly as C++)
# -----------------------------------------------------------
display_current_settings() {
    echo ""
    echo "Current Settings:"
    echo "Target Drive: ${TARGET_DRIVE:-[Not Set]}"
    echo "Filesystem: ${FILESYSTEM_TYPE:-[Not Set]}"
    echo "Username: ${NEW_USERNAME:-[Not Set]}"
    echo "Root Password: $([ -n "$ROOT_PASSWORD" ] && echo '[Set]' || echo '[Not Set]')"
    echo "User Password: $([ -n "$USER_PASSWORD" ] && echo '[Set]' || echo '[Not Set]')"
    echo "Timezone: ${TIMEZONE:-[Not Set]}"
    echo "Keyboard Layout: ${KEYBOARD_LAYOUT:-[Not Set]}"
    echo "Current Distro: ${CURRENT_DISTRO_NAME:-[Not Set]}"
    echo "Extra Packages: ${EXTRA_PACKAGES:-[Not Set]}"
    echo ""
}

# -----------------------------------------------------------
# Main Menu (exactly as C++)
# -----------------------------------------------------------
show_main_menu() {
    while true; do
        clear
        # ASCII art header
        echo "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗"
        echo "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝"
        echo "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░"
        echo "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗"
        echo "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝"
        echo "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝"
        echo "claudemods distribution iso creator Beta v1.01 06-06-2026"
        echo ""
        
        display_current_settings
        
        local settings_text
        settings_text="Target Drive: ${TARGET_DRIVE:-[Not Set]}\n"
        settings_text+="Filesystem: ${FILESYSTEM_TYPE:-[Not Set]}\n"
        settings_text+="Username: ${NEW_USERNAME:-[Not Set]}\n"
        settings_text+="Root Password: $([ -n "$ROOT_PASSWORD" ] && echo '[Set]' || echo '[Not Set]')\n"
        settings_text+="User Password: $([ -n "$USER_PASSWORD" ] && echo '[Set]' || echo '[Not Set]')\n"
        settings_text+="Timezone: ${TIMEZONE:-[Not Set]}\n"
        settings_text+="Keyboard Layout: ${KEYBOARD_LAYOUT:-[Not Set]}\n"
        settings_text+="Current Distro: ${CURRENT_DISTRO_NAME:-[Not Set]}\n"
        settings_text+="Extra Packages: ${EXTRA_PACKAGES:-[Not Set]}"
        
        local choice
        choice=$(zenity --list --title="claudemods distribution iso creator" \
            --text="Current Settings:\n\n$settings_text" \
            --column="Option" \
            "Setup Bootloader and Drive" \
            "Set Username" \
            "Set Root Password" \
            "Set User Password" \
            "Set Timezone" \
            "Set Keyboard Layout" \
            "Set Wireless Regdom" \
            "Select Distro to Install" \
            "Install Extra Packages" \
            "Start Installation" \
            "Exit" \
            --width=600 --height=500 2>/dev/null)
        
        case "$choice" in
            "Setup Bootloader and Drive") setup_bootloader_and_drive ;;
            "Set Username") set_username ;;
            "Set Root Password") set_root_password ;;
            "Set User Password") set_user_password ;;
            "Set Timezone") set_timezone ;;
            "Set Keyboard Layout") set_keyboard_layout ;;
            "Set Wireless Regdom") set_wireless_regdom ;;
            "Select Distro to Install") show_distro_selection ;;
            "Install Extra Packages") set_extra_packages ;;
            "Start Installation") start_installation ;;
            "Exit"|"")
                echo "Exiting. Goodbye!"
                exit 0
                ;;
        esac
    done
}

# -----------------------------------------------------------
# Run (exactly as C++ run())
# -----------------------------------------------------------
run() {
    load_configuration
    
    if ! extract_required_files; then
        echo "Failed to extract required files. Cannot continue."
        zenity --error --title="Error" --text="Failed to extract required files. Cannot continue." --width=400 2>/dev/null
        return
    fi
    
    show_main_menu
}

# -----------------------------------------------------------
# Main Entry Point
# -----------------------------------------------------------
if ! command -v zenity &>/dev/null; then
    echo "Error: zenity is not installed. Install with: sudo apt install zenity"
    exit 1
fi

if [[ $EUID -ne 0 ]]; then
    zenity --error --title="Error" --text="This script must be run as root (sudo)." --width=400 2>/dev/null
    exit 1
fi

run
