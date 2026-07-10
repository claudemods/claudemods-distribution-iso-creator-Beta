#!/bin/bash

# ============================================================
# Claudemods Distribution ISO Creator - Zenity GUI Version
# Full conversion from C++ to Bash + Zenity
# ============================================================

set -euo pipefail

# -----------------------------------------------------------
# Global Variables
# -----------------------------------------------------------
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
# Utility Functions
# -----------------------------------------------------------
get_config_file_path() {
    echo "$CURRENT_DIR/configurationclaudemods.txt"
}

get_current_dir() {
    echo "$(pwd)"
}

get_target_folder() {
    echo "/mnt"
}

get_calamares_folder() {
    echo "$CURRENT_DIR/calamares-claudemods"
}

directory_exists() {
    [[ -d "$1" ]]
}

file_exists() {
    [[ -f "$1" ]]
}

is_block_device() {
    [[ -b "$1" ]]
}

exec_cmd() {
    eval "$1" 2>/dev/null || true
}

# -----------------------------------------------------------
# Save / Load Configuration
# -----------------------------------------------------------
save_configuration() {
    cat > "$CONFIG_FILE" <<EOF
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
    zenity --info --title="Configuration" --text="Configuration saved to $CONFIG_FILE" --width=400 2>/dev/null
}

load_configuration() {
    if [[ -f "$CONFIG_FILE" ]]; then
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
        done < "$CONFIG_FILE"
        zenity --info --title="Configuration" --text="Configuration loaded from $CONFIG_FILE" --width=400 2>/dev/null
    else
        zenity --info --title="Configuration" --text="No existing configuration found. Starting with default settings." --width=400 2>/dev/null
    fi
}

# -----------------------------------------------------------
# Extract Required Files
# -----------------------------------------------------------
extract_required_files() {
    zenity --info --title="Checking Files" --text="Checking for required folders..." --width=400 2>/dev/null
    
    local calamares_folder
    calamares_folder=$(get_calamares_folder)
    
    local build_image_exists=false
    local calamares_files_exists=false
    local working_hooks_exists=false
    
    [[ -d "$calamares_folder/build-image-arch-img" ]] && build_image_exists=true
    [[ -d "$calamares_folder/calamares-files" ]] && calamares_files_exists=true
    [[ -d "$calamares_folder/working-hooks-btrfs-ext4" ]] && working_hooks_exists=true
    
    if $build_image_exists && $calamares_files_exists && $working_hooks_exists; then
        zenity --info --title="Files Check" --text="All required folders already exist." --width=400 2>/dev/null
        return 0
    fi
    
    if [[ ! -d "$calamares_folder" ]]; then
        zenity --info --title="Creating Folder" --text="Creating calamares-claudemods folder..." --width=400 2>/dev/null
        sudo mkdir -p "$calamares_folder"
    fi
    
    local source_dir="$CURRENT_DIR/calamares-per-distro/claudemods"
    local zip_files=(
        "calamares-claudemods.zip"
        "claudemods.zip"
        "build-image-claudemods.zip"
    )
    
    for zip_file in "${zip_files[@]}"; do
        local source_path="$source_dir/$zip_file"
        if [[ -f "$source_path" ]]; then
            zenity --info --title="Extracting" --text="Extracting $zip_file..." --width=400 2>/dev/null
            sudo unzip -q "$source_path" -d "$calamares_folder" >/dev/null 2>&1
        fi
    done
    
    zenity --info --title="Complete" --text="Extraction process completed." --width=400 2>/dev/null
    return 0
}

# -----------------------------------------------------------
# System Command Execution
# -----------------------------------------------------------
execute_command() {
    local cmd="$1"
    echo "Executing: $cmd"
    eval "$cmd" 2>/dev/null || true
}

execute_command_with_error() {
    local cmd="$1"
    local error_msg="$2"
    echo "Executing: $cmd"
    if ! eval "$cmd" 2>/dev/null; then
        zenity --error --title="Error" --text="$error_msg" --width=400 2>/dev/null
        return 1
    fi
    return 0
}

# -----------------------------------------------------------
# Setup Target Directory
# -----------------------------------------------------------
setup_target_directory() {
    local target_folder="$1"
    local current_dir
    current_dir=$(get_current_dir)
    
    zenity --info --title="Setup" --text="Creating target directory: $target_folder" --width=400 2>/dev/null
    
    execute_command "sudo mkdir -p $target_folder"
    
    if [[ ! -d "$target_folder" ]]; then
        zenity --error --title="Error" --text="Failed to create target directory: $target_folder" --width=400 2>/dev/null
        return 1
    fi
    
    execute_command "sudo mkdir -p $target_folder/usr/"
    execute_command "sudo mkdir -p $target_folder/usr/lib"
    execute_command "sudo mkdir -p $target_folder/usr/lib/initcpio/"
    execute_command "sudo mkdir -p $target_folder/usr/lib/initcpio/udev/"
    execute_command "sudo mkdir -p $target_folder/etc/sysctl.d"
    execute_command "sudo cp -r $current_dir/needed-files/11-dm-initramfs.rules $target_folder/usr/lib/initcpio/udev/11-dm-initramfs.rules"
    execute_command "sudo cp -r $current_dir/needed-files/11-dm-initramfs.rules /usr/lib/initcpio/udev/11-dm-initramfs.rules"
    execute_command "sudo cp -r $current_dir/needed-files/80-gamecompatibility.conf $target_folder/etc/sysctl.d/80-gamecompatibility.conf"
    execute_command "sudo mkdir -p $target_folder/etc/pacman.d"
    execute_command "sudo mkdir -p $target_folder/boot/grub"
    execute_command "sudo mkdir -p $target_folder/usr/share/grub/themes"
    execute_command "sudo mkdir -p $target_folder/usr/share/plymouth/themes"
    execute_command "sudo mkdir -p $target_folder/usr/local/bin"
    execute_command "sudo mkdir -p $target_folder/etc/systemd/system"
    execute_command "sudo mkdir -p $target_folder/etc/sddm.conf.d"
    
    zenity --info --title="Success" --text="Target directory created successfully!" --width=400 2>/dev/null
    return 0
}

# -----------------------------------------------------------
# Setup Pacman and Files
# -----------------------------------------------------------
setup_pacman_and_files() {
    local target_folder="$1"
    local current_dir
    current_dir=$(get_current_dir)
    
    execute_command "sudo cp -r $current_dir/needed-files/vconsole.conf $target_folder/etc/vconsole.conf"
    execute_command "sudo cp -r /etc/resolv.conf $target_folder/etc/resolv.conf"
    execute_command "sudo unzip -o $current_dir/needed-files/pacman.d.zip -d $target_folder/etc/pacman.d"
    execute_command "sudo unzip -o $current_dir/needed-files/pacman.d.zip -d /etc/pacman.d"
    execute_command "sudo cp -r $current_dir/needed-files/pacman.conf $target_folder/etc/pacman.conf"
    execute_command "sudo cp -r $current_dir/needed-files/pacman.conf /etc/pacman.conf"
    execute_command "sudo pacman -Sy"
    execute_command "sudo pacman -S archlinux-keyring"
    execute_command "sudo pacman-key --populate"
    execute_command "sudo pacman-key --init"
}

# -----------------------------------------------------------
# Verify Pacstrap Success
# -----------------------------------------------------------
verify_pacstrap_success() {
    local target_folder="$1"
    local test_bin="$target_folder/bin/bash"
    
    if [[ ! -f "$test_bin" ]]; then
        zenity --error --title="Error" --text="pacstrap failed! /bin/bash not found in target." --width=400 2>/dev/null
        return 1
    fi
    return 0
}

# -----------------------------------------------------------
# Setup Boot Directory
# -----------------------------------------------------------
setup_boot_directory() {
    local target_folder="$1"
    execute_command "sudo mkdir -p $target_folder/boot"
    execute_command "sudo mkdir -p $target_folder/boot/grub"
    execute_command "sudo touch $target_folder/boot/grub/grub.cfg.new"
}

# -----------------------------------------------------------
# Create User Home Structure
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
# Fix User Places XBEL
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
# Install Calamares
# -----------------------------------------------------------
install_calamares() {
    local target_folder="$1"
    local current_dir
    current_dir=$(get_current_dir)
    
    zenity --info --title="Installing" --text="Installing Calamares installer and setting up iso ..." --width=400 2>/dev/null
    
    execute_command "sudo cp $current_dir/needed-files/kwalletrc $target_folder/home/$NEW_USERNAME/.config/kwalletrc"
    execute_command "sudo rm -rf $target_folder/home/$NEW_USERNAME/.local/share/kwalletd/*"
    execute_command "sudo cp -r $current_dir/needed-files/wireless-regdom $target_folder/etc/conf.d/wireless-regdom"
    execute_command "setfattr -n user.kde.fm.viewproperties#1 -v '[Dolphin]\\012Timestamp=2026,1,20,17,27,36.341\\012Version=4\\012\\012[Settings]\\012HiddenFilesShown=true' $target_folder/home/$NEW_USERNAME/.local/share/dolphin/view_properties/global"
    
    zenity --info --title="Complete" --text="Installation completed!" --width=400 2>/dev/null
}

# -----------------------------------------------------------
# Setup Bootloader and Drive
# -----------------------------------------------------------
setup_bootloader_and_drive() {
    # Show available drives
    local drives_list
    drives_list=$(lsblk -d -o NAME,SIZE,MODEL 2>/dev/null | grep -v 'loop\|sr0\|zram' | awk '{print "/dev/"$1, $2, $3, $4}')
    
    zenity --info --title="Available Drives" --text="Available drives:\n\n$drives_list\n\nWARNING: The selected drive will be COMPLETELY ERASED!" --width=600 2>/dev/null
    
    # Select target drive
    TARGET_DRIVE=$(zenity --entry --title="Target Drive" --text="Enter target drive (e.g., /dev/sda):" --entry-text="$TARGET_DRIVE" --width=400 2>/dev/null)
    
    if [[ -z "$TARGET_DRIVE" ]]; then
        zenity --info --title="No Change" --text="No drive selected. Keeping current setting." --width=400 2>/dev/null
    elif ! is_block_device "$TARGET_DRIVE"; then
        zenity --error --title="Error" --text="$TARGET_DRIVE is not a valid block device!" --width=400 2>/dev/null
    else
        zenity --info --title="Drive Set" --text="Target drive set to: $TARGET_DRIVE" --width=400 2>/dev/null
    fi
    
    # Select filesystem type
    if [[ -n "$TARGET_DRIVE" ]]; then
        local fs_choice
        fs_choice=$(zenity --list --title="Filesystem Type" \
            --text="Select filesystem type:" \
            --column="Filesystem" \
            "Btrfs (with subvolumes, compression, snapshots support)" \
            "Ext4 (standard filesystem)" \
            --width=600 --height=200 2>/dev/null)
        
        if [[ "$fs_choice" == "Btrfs (with subvolumes, compression, snapshots support)" ]]; then
            FILESYSTEM_TYPE="btrfs"
            zenity --info --title="Filesystem" --text="Filesystem set to: Btrfs\nBtrfs subvolumes will be created: @, @home, @root, @srv, @cache, @tmp, @log\nCompression: zstd level 22" --width=400 2>/dev/null
        else
            FILESYSTEM_TYPE="ext4"
            zenity --info --title="Filesystem" --text="Filesystem set to: Ext4" --width=400 2>/dev/null
        fi
    fi
    
    # Show summary
    if [[ -n "$TARGET_DRIVE" && -n "$FILESYSTEM_TYPE" ]]; then
        zenity --warning --title="Drive Configuration Summary" \
            --text="Drive Configuration Summary:\n\nDrive: $TARGET_DRIVE\nPartition 1: ${TARGET_DRIVE}1 (EFI - FAT32, 550MB)\nPartition 2: ${TARGET_DRIVE}2 (Root - $( [[ "$FILESYSTEM_TYPE" == "btrfs" ]] && echo "Btrfs" || echo "Ext4" ))\nBootloader: GRUB (UEFI)\n\nWARNING: ALL DATA ON $TARGET_DRIVE WILL BE DESTROYED!" \
            --width=500 2>/dev/null
    fi
    
    save_configuration
}

# -----------------------------------------------------------
# Prepare Target Partitions
# -----------------------------------------------------------
prepare_target_partitions() {
    zenity --info --title="Preparing" --text="Preparing target partitions on $TARGET_DRIVE..." --width=400 2>/dev/null
    
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
    
    if ! is_block_device "$efi_part" || ! is_block_device "$root_part"; then
        zenity --error --title="Error" --text="Failed to create partitions" --width=400 2>/dev/null
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
# Setup Btrfs Subvolumes
# -----------------------------------------------------------
setup_btrfs_subvolumes() {
    local root_part="${TARGET_DRIVE}2"
    
    zenity --info --title="Btrfs Setup" --text="Setting up Btrfs subvolumes with zstd:22 compression..." --width=400 2>/dev/null
    
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
# Setup Ext4 Filesystem
# -----------------------------------------------------------
setup_ext4_filesystem() {
    local root_part="${TARGET_DRIVE}2"
    
    zenity --info --title="Ext4 Setup" --text="Setting up Ext4 filesystem..." --width=400 2>/dev/null
    
    execute_command "sudo mount $root_part /mnt"
    execute_command "sudo mkdir -p /mnt/{home,boot/efi,etc,usr,var,proc,sys,dev,tmp,run}"
}

# -----------------------------------------------------------
# Mount / Unmount System Dirs
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
    zenity --info --title="Unmounting" --text="Unmounting target filesystems..." --width=400 2>/dev/null
    execute_command "sudo umount -l /mnt 2>/dev/null || true"
}

# -----------------------------------------------------------
# Create User
# -----------------------------------------------------------
create_user() {
    execute_command "sudo chroot /mnt /bin/bash -c \"useradd -m -G wheel -s /bin/bash $NEW_USERNAME\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'root:$ROOT_PASSWORD' | chpasswd\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo '$NEW_USERNAME:$USER_PASSWORD' | chpasswd\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\""
}

# -----------------------------------------------------------
# Apply Timezone and Keyboard Settings
# -----------------------------------------------------------
apply_timezone_keyboard_settings() {
    zenity --info --title="Settings" --text="Setting timezone to: $TIMEZONE" --width=400 2>/dev/null
    execute_command "sudo chroot /mnt /bin/bash -c \"ln -sf /usr/share/zoneinfo/$TIMEZONE /etc/localtime\""
    execute_command "sudo chroot /mnt /bin/bash -c \"hwclock --systohc\""
    
    zenity --info --title="Settings" --text="Setting keyboard layout to: $KEYBOARD_LAYOUT" --width=400 2>/dev/null
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'KEYMAP=$KEYBOARD_LAYOUT' > /etc/vconsole.conf\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\""
    execute_command "sudo chroot /mnt /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\""
    execute_command "sudo chroot /mnt /bin/bash -c \"locale-gen\""
}

# -----------------------------------------------------------
# Install GRUB
# -----------------------------------------------------------
install_grub() {
    zenity --info --title="GRUB" --text="Installing GRUB bootloader..." --width=400 2>/dev/null
    
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
# Post Install Menu
# -----------------------------------------------------------
post_install_menu() {
    local choice
    choice=$(zenity --list --title="Installation Complete" \
        --text="INSTALLATION COMPLETE\n\nChoose an option:" \
        --column="Option" \
        "Reboot now" \
        "Exit to shell" \
        --width=400 --height=200 2>/dev/null)
    
    if [[ "$choice" == "Reboot now" ]]; then
        zenity --info --title="Rebooting" --text="Unmounting and rebooting..." --width=400 2>/dev/null
        unmount_target
        sudo reboot
    else
        zenity --info --title="Shell" --text="Exiting to shell. System is still mounted at /mnt" --width=400 2>/dev/null
    fi
}

# -----------------------------------------------------------
# Check Settings Configured
# -----------------------------------------------------------
check_settings_configured() {
    local errors=""
    
    [[ -z "$TARGET_DRIVE" ]] && errors+="Target drive not set!\n"
    [[ -z "$FILESYSTEM_TYPE" ]] && errors+="Filesystem type not set!\n"
    [[ -z "$NEW_USERNAME" ]] && errors+="Username not set!\n"
    [[ -z "$ROOT_PASSWORD" ]] && errors+="Root password not set!\n"
    [[ -z "$USER_PASSWORD" ]] && errors+="User password not set!\n"
    [[ -z "$TIMEZONE" ]] && errors+="Timezone not set!\n"
    [[ -z "$KEYBOARD_LAYOUT" ]] && errors+="Keyboard layout not set!\n"
    [[ -z "$CURRENT_DISTRO_NAME" ]] && errors+="No distribution selected!\n"
    
    if [[ -n "$errors" ]]; then
        zenity --error --title="Configuration Error" --text="Cannot proceed with installation:\n\n$errors" --width=400 2>/dev/null
        return 1
    fi
    return 0
}

# -----------------------------------------------------------
# Distro-Specific Install Functions
# -----------------------------------------------------------
install_spitfire_ckge_minimal() {
    zenity --info --title="Installing" --text="Installing Spitfire CKGE Minimal to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    zenity --info --title="Pacstrap" --text="Installing base system with pacstrap..." --width=400 2>/dev/null
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop calamares-fix protonup-qt hhd adjustor hhd-ui sddm piper"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/spitfire-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $current_dir/spitfire-minimal-v1.01.zip"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_spitfire_ckge_minimal_dev() {
    zenity --info --title="Installing" --text="Installing Spitfire CKGE Minimal Dev to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-dev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/spitfire-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $current_dir/spitfire-minimal-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_spitfire_ckge_full() {
    zenity --info --title="Installing" --text="Installing Spitfire CKGE Full to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/spitfire-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $current_dir/spitfire-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_spitfire_ckge_full_dev() {
    zenity --info --title="Installing" --text="Installing Spitfire CKGE Full Dev to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/spitfire-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $current_dir/spitfire-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_spitfire_ckge_black_full() {
    zenity --info --title="Installing" --text="Installing Spitfire CKGE Black Full to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-black-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-CKGBE-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/spitfire-full-black-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo rm -rf $current_dir/spitfire-full-black-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_spitfire_ckge_black_full_dev() {
    zenity --info --title="Installing" --text="Installing Spitfire CKGE Black Full Dev to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-black-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-CKGBE-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/spitfire-full-black-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksspitfire.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksspitfire.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-CKGBE-v1.01.zip"
    execute_command "sudo rm -rf $current_dir/spitfire-full-black-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksspitfire.sh"
    
    fix_user_places_xbel "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_apex_ckge_minimal() {
    zenity --info --title="Installing" --text="Installing Apex CKGE Minimal to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop calamares-fix protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/apex-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $current_dir/apex-minimal-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_apex_ckge_minimal_dev() {
    zenity --info --title="Installing" --text="Installing Apex CKGE Minimal Dev to $TARGET_DRIVE..." --width=400 2>/dev/null    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-dev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm piper"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/term.service /mnt/etc/systemd/system/term.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-minimal.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/apex-minimal-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-minimal.zip"
    execute_command "sudo rm -rf $current_dir/apex-minimal-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_apex_ckge_full() {
    zenity --info --title="Installing" --text="Installing Apex CKGE Full to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/apex-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $current_dir/apex-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

install_apex_ckge_full_dev() {
    zenity --info --title="Installing" --text="Installing Apex CKGE Full Dev to $TARGET_DRIVE..." --width=400 2>/dev/null
    
    local target_folder="/mnt"
    local current_dir
    current_dir=$(get_current_dir)
    
    if ! setup_target_directory "$target_folder"; then return 1; fi
    setup_pacman_and_files "$target_folder"
    
    local pacstrap_cmd="sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm"
    [[ -n "$EXTRA_PACKAGES" ]] && pacstrap_cmd+=" $EXTRA_PACKAGES"
    execute_command "$pacstrap_cmd"
    
    if ! verify_pacstrap_success "$target_folder"; then return 1; fi
    
    setup_boot_directory "$target_folder"
    mount_system_dirs
    
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\""
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\""
    apply_timezone_keyboard_settings
    
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes"
    execute_command "sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\""
    execute_command "sudo cp -r $current_dir/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service"
    execute_command "sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\""
    execute_command "sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\""
    
    create_user
    create_user_home_structure "$target_folder"
    
    execute_command "cd /mnt"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full-v1.01.zip"
    execute_command "sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip"
    execute_command "sudo unzip -o $current_dir/Arch-Systemtool-v1.01.zip -d /mnt/opt"
    execute_command "sudo unzip -o $current_dir/apex-full-v1.01.zip -d /mnt/home/$NEW_USERNAME/"
    execute_command "sudo mkdir -p /mnt/etc/sddm.conf.d"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf"
    execute_command "sudo cp $current_dir/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh"
    execute_command "sudo chmod +x /mnt/opt/tweaksapex.sh"
    execute_command "sudo chroot /mnt /bin/bash -c \"su - $NEW_USERNAME -c 'cd /opt && ./tweaksapex.sh $NEW_USERNAME'\""
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/konsolerc /mnt/home/$NEW_USERNAME/.config/konsolerc"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.colorscheme"
    execute_command "sudo cp -r $current_dir/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/$NEW_USERNAME/.local/share/konsole/claudemods-cyan.profile"
    execute_command "sudo rm -rf $current_dir/Arch-Systemtool-v1.01.zip"
    execute_command "sudo rm -rf $current_dir/apex-full-v1.01.zip"
    execute_command "sudo rm -rf /mnt/opt/tweaksapex.sh"
    
    fix_user_places_xbel_apex "$target_folder"
    install_calamares "$target_folder"
    unmount_system_dirs
    install_grub
    
    zenity --info --title="Complete" --text="Installation complete! System installed to $TARGET_DRIVE" --width=400 2>/dev/null
    post_install_menu
}

# -----------------------------------------------------------
# Start Installation
# -----------------------------------------------------------
start_installation() {
    if ! check_settings_configured; then
        return
    fi
    
    # Confirm installation
    if ! zenity --question --title="Confirm Installation" \
        --text="Ready to install $CURRENT_DISTRO_NAME to $TARGET_DRIVE\n\nALL DATA ON $TARGET_DRIVE WILL BE DESTROYED!\n\nProceed?" \
        --width=500 2>/dev/null; then
        return
    fi
    
    # Progress dialog
    (
        echo "10"; echo "# Preparing partitions..."
        prepare_target_partitions
        
        echo "20"
        if [[ "$FILESYSTEM_TYPE" == "btrfs" ]]; then
            echo "# Setting up Btrfs subvolumes..."
            setup_btrfs_subvolumes
        else
            echo "# Setting up Ext4 filesystem..."
            setup_ext4_filesystem
        fi
        
        echo "30"; echo "# Installing distribution..."
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
        
        echo "100"; echo "# Installation complete!"
    ) | zenity --progress --title="Installation Progress" \
        --text="Starting installation..." \
        --percentage=0 \
        --auto-close \
        --width=500 2>/dev/null
}

# -----------------------------------------------------------
# Set Wireless Regdom
# -----------------------------------------------------------
set_wireless_regdom() {
    local current_dir
    current_dir=$(get_current_dir)
    local wireless_regdom_file="$current_dir/needed-files/wireless-regdom"
    
    zenity --info --title="Wireless Regdom" \
        --text="Opening wireless regulatory domain file for editing...\n\nFile: $wireless_regdom_file" \
        --width=500 2>/dev/null
    
    if [[ -n "$EDITOR" ]]; then
        $EDITOR "$wireless_regdom_file"
    elif command -v nano &>/dev/null; then
        x-terminal-emulator -e "nano $wireless_regdom_file" 2>/dev/null || nano "$wireless_regdom_file"
    elif command -v gedit &>/dev/null; then
        gedit "$wireless_regdom_file"
    else
        zenity --text-info --filename="$wireless_regdom_file" --editable --title="Wireless Regdom" --width=600 --height=400 2>/dev/null
    fi
    
    zenity --info --title="Done" --text="Wireless regulatory domain file updated." --width=400 2>/dev/null
}

# -----------------------------------------------------------
# Set Extra Packages
# -----------------------------------------------------------
set_extra_packages() {
    EXTRA_PACKAGES=$(zenity --entry --title="Extra Packages" \
        --text="Enter extra packages (space separated):" \
        --entry-text="$EXTRA_PACKAGES" \
        --width=500 2>/dev/null)
    save_configuration
}

# -----------------------------------------------------------
# Distro Selection Menu
# -----------------------------------------------------------
show_distro_selection() {
    local choice
    choice=$(zenity --list --title="Select Distribution" \
        --text="Choose a distribution to install:" \
        --column="Distribution" \
        "Spitfire CKGE Minimal" \
        "Spitfire CKGE Minimal Dev" \
        "Spitfire CKGE Full" \
        "Spitfire CKGE Full Dev" \
        "Spitfire CKGE Black Full" \
        "Spitfire CKGE Black Full Dev" \
        "Apex CKGE Minimal" \
        "Apex CKGE Minimal Dev" \
        "Apex CKGE Full" \
        "Apex CKGE Full Dev" \
        --width=500 --height=400 2>/dev/null)
    
    case "$choice" in
        "Spitfire CKGE Minimal") CURRENT_DISTRO_NAME="Spitfire-CKGE-Minimal" ;;
        "Spitfire CKGE Minimal Dev") CURRENT_DISTRO_NAME="Spitfire-CKGE-Minimal-Dev" ;;
        "Spitfire CKGE Full") CURRENT_DISTRO_NAME="Spitfire-CKGE-Full" ;;
        "Spitfire CKGE Full Dev") CURRENT_DISTRO_NAME="Spitfire-CKGE-Full-Dev" ;;
        "Spitfire CKGE Black Full") CURRENT_DISTRO_NAME="Spitfire-CKGE-Black-Full" ;;
        "Spitfire CKGE Black Full Dev") CURRENT_DISTRO_NAME="Spitfire-CKGE-Black-Full-Dev" ;;
        "Apex CKGE Minimal") CURRENT_DISTRO_NAME="Apex-CKGE-Minimal" ;;
        "Apex CKGE Minimal Dev") CURRENT_DISTRO_NAME="Apex-CKGE-Minimal-Dev" ;;
        "Apex CKGE Full") CURRENT_DISTRO_NAME="Apex-CKGE-Full" ;;
        "Apex CKGE Full Dev") CURRENT_DISTRO_NAME="Apex-CKGE-Full-Dev" ;;
        *) return ;;
    esac
    
    save_configuration
    zenity --info --title="Distribution Selected" \
        --text="$choice selected.\n\nUse 'Start Installation' to begin." \
        --width=400 2>/dev/null
}

# -----------------------------------------------------------
# Display Current Settings
# -----------------------------------------------------------
display_current_settings() {
    local settings_text=""
    settings_text+="Target Drive: ${TARGET_DRIVE:-[Not Set]}\n"
    settings_text+="Filesystem: ${FILESYSTEM_TYPE:-[Not Set]}\n"
    settings_text+="Username: ${NEW_USERNAME:-[Not Set]}\n"
    settings_text+="Root Password: ${ROOT_PASSWORD:+[Set]}${ROOT_PASSWORD:-[Not Set]}\n"
    settings_text+="User Password: ${USER_PASSWORD:+[Set]}${USER_PASSWORD:-[Not Set]}\n"
    settings_text+="Timezone: ${TIMEZONE:-[Not Set]}\n"
    settings_text+="Keyboard Layout: ${KEYBOARD_LAYOUT:-[Not Set]}\n"
    settings_text+="Current Distro: ${CURRENT_DISTRO_NAME:-[Not Set]}\n"
    settings_text+="Extra Packages: ${EXTRA_PACKAGES:-[Not Set]}"
    
    echo "$settings_text"
}

# -----------------------------------------------------------
# Main Menu
# -----------------------------------------------------------
show_main_menu() {
    while true; do
        local settings
        settings=$(display_current_settings)
        
        local choice
        choice=$(zenity --list --title="Claudemods Distribution ISO Creator v1.01" \
            --text="Current Settings:\n\n$settings\n\nSelect an option:" \
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
            "Set Username")
                NEW_USERNAME=$(zenity --entry --title="Username" --text="Enter username:" --entry-text="$NEW_USERNAME" --width=400 2>/dev/null)
                save_configuration
                ;;
            "Set Root Password")
                ROOT_PASSWORD=$(zenity --password --title="Root Password" --text="Enter root password:" --width=400 2>/dev/null)
                save_configuration
                ;;
            "Set User Password")
                USER_PASSWORD=$(zenity --password --title="User Password" --text="Enter user password:" --width=400 2>/dev/null)
                save_configuration
                ;;
            "Set Timezone")
                local tz_choice
                tz_choice=$(zenity --list --title="Select Timezone" \
                    --column="Timezone" \
                    "America/New_York" \
                    "Europe/London" \
                    "Europe/Berlin" \
                    "Europe/Paris" \
                    "Europe/Madrid" \
                    "Europe/Rome" \
                    "Asia/Tokyo" \
                    "Other (manual entry)" \
                    --width=400 --height=350 2>/dev/null)
                if [[ "$tz_choice" == "Other (manual entry)" ]]; then
                    TIMEZONE=$(zenity --entry --title="Timezone" --text="Enter timezone (e.g., Europe/Berlin):" --width=400 2>/dev/null)
                elif [[ -n "$tz_choice" ]]; then
                    TIMEZONE="$tz_choice"
                fi
                save_configuration
                ;;
            "Set Keyboard Layout")
                local kb_choice
                kb_choice=$(zenity --list --title="Select Keyboard Layout" \
                    --column="Layout" \
                    "us" "uk" "de" "fr" "es" "it" "jp" \
                    "Other (manual entry)" \
                    --width=400 --height=350 2>/dev/null)
                if [[ "$kb_choice" == "Other (manual entry)" ]]; then
                    KEYBOARD_LAYOUT=$(zenity --entry --title="Keyboard Layout" --text="Enter keyboard layout (e.g., br, ru, pt):" --width=400 2>/dev/null)
                elif [[ -n "$kb_choice" ]]; then
                    KEYBOARD_LAYOUT="$kb_choice"
                fi
                save_configuration
                ;;
            "Set Wireless Regdom") set_wireless_regdom ;;
            "Select Distro to Install") show_distro_selection ;;
            "Install Extra Packages") set_extra_packages ;;
            "Start Installation") start_installation ;;
            "Exit"|"") 
                zenity --info --title="Goodbye" --text="Exiting. Goodbye!" --width=300 2>/dev/null
                exit 0
                ;;
        esac
    done
}

# -----------------------------------------------------------
# Main Entry Point
# -----------------------------------------------------------
main() {
    # Check if zenity is installed
    if ! command -v zenity &>/dev/null; then
        echo "Error: zenity is not installed. Please install it first."
        echo "  Debian/Ubuntu: sudo apt install zenity"
        echo "  Arch: sudo pacman -S zenity"
        echo "  Fedora: sudo dnf install zenity"
        exit 1
    fi
    
    # Check if running as root (required for most operations)
    if [[ $EUID -ne 0 ]]; then
        zenity --error --title="Error" --text="This script must be run as root (sudo)." --width=400 2>/dev/null
        exit 1
    fi
    
    load_configuration
    
    if ! extract_required_files; then
        zenity --error --title="Error" --text="Failed to extract required files. Cannot continue." --width=400 2>/dev/null
        exit 1
    fi
    
    show_main_menu
}

# Run the main function
main "$@"
