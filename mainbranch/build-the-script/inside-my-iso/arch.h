
#ifndef ARCH_H
#define ARCH_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <termios.h>
#include <algorithm>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

extern const std::string COLOR_CYAN;
extern const std::string COLOR_RED;
extern const std::string COLOR_GREEN;
extern const std::string COLOR_YELLOW;
extern const std::string COLOR_ORANGE;
extern const std::string COLOR_PURPLE;
extern const std::string COLOR_RESET;

class ArchInstaller {
private:
    std::string new_username;
    std::string root_password;
    std::string user_password;
    std::string timezone;
    std::string keyboard_layout;
    std::string current_distro_name;
    std::string extra_packages;
    std::string selected_kernel;
    std::string current_desktop_name;
    std::string target_drive;
    std::string filesystem_type;

    struct termios oldt, newt;

    std::string getConfigFilePath() {
        return getCurrentDir() + "/configurationarch.txt";
    }

    void saveConfiguration() {
        std::string configFile = getConfigFilePath();
        std::ofstream file(configFile);
        if (file.is_open()) {
            file << "username=" << new_username << std::endl;
            file << "root_password=" << root_password << std::endl;
            file << "user_password=" << user_password << std::endl;
            file << "timezone=" << timezone << std::endl;
            file << "keyboard_layout=" << keyboard_layout << std::endl;
            file << "current_distro=" << current_distro_name << std::endl;
            file << "extra_packages=" << extra_packages << std::endl;
            file << "selected_kernel=" << selected_kernel << std::endl;
            file << "current_desktop=" << current_desktop_name << std::endl;
            file << "target_drive=" << target_drive << std::endl;
            file << "filesystem_type=" << filesystem_type << std::endl;
            file.close();
            std::cout << COLOR_GREEN << "Configuration saved to " << configFile << COLOR_RESET << std::endl;
        } else {
            std::cerr << COLOR_RED << "Failed to save configuration to " << configFile << COLOR_RESET << std::endl;
        }
    }

    void loadConfiguration() {
        std::string configFile = getConfigFilePath();
        std::ifstream file(configFile);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                size_t delimiter = line.find('=');
                if (delimiter != std::string::npos) {
                    std::string key = line.substr(0, delimiter);
                    std::string value = line.substr(delimiter + 1);
                    if (key == "username") new_username = value;
                    else if (key == "root_password") root_password = value;
                    else if (key == "user_password") user_password = value;
                    else if (key == "timezone") timezone = value;
                    else if (key == "keyboard_layout") keyboard_layout = value;
                    else if (key == "current_distro") current_distro_name = value;
                    else if (key == "extra_packages") extra_packages = value;
                    else if (key == "selected_kernel") selected_kernel = value;
                    else if (key == "current_desktop") current_desktop_name = value;
                    else if (key == "target_drive") target_drive = value;
                    else if (key == "filesystem_type") filesystem_type = value;
                }
            }
            file.close();
            std::cout << COLOR_GREEN << "Configuration loaded from " << configFile << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_YELLOW << "No existing configuration found. Starting with default settings." << COLOR_RESET << std::endl;
        }
    }

    std::string getCurrentDir() {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            return std::string(cwd);
        }
        return ".";
    }

    std::string getTargetFolder() {
        return "/mnt";
    }

    std::string getFullTargetPath() {
        return getTargetFolder();
    }

    std::string getCalamaresFolder() {
        return getCurrentDir() + "/calamares-arch";
    }

    bool directoryExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
    }

    bool fileExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0);
    }

    bool is_block_device(const std::string& path) {
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) != 0) return false;
        return S_ISBLK(statbuf.st_mode);
    }

    std::string exec_cmd(const char* cmd) {
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(cmd, "r");
        if (!pipe) return "";
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
        pclose(pipe);
        return result;
    }

    bool extractRequiredFiles() {
        std::cout << COLOR_CYAN << "Checking for required folders..." << COLOR_RESET << std::endl;
        std::string currentDir = getCurrentDir();
        std::string calamaresFolder = getCalamaresFolder();
        bool buildImageExists = directoryExists(calamaresFolder + "/build-image-arch-img");
        bool calamaresFilesExists = directoryExists(calamaresFolder + "/calamares-files");
        bool workingHooksExists = directoryExists(calamaresFolder + "/working-hooks-btrfs-ext4");
        if (buildImageExists && calamaresFilesExists && workingHooksExists) {
            std::cout << COLOR_GREEN << "All required folders already exist." << COLOR_RESET << std::endl;
            return true;
        }
        if (!directoryExists(calamaresFolder)) {
            std::cout << COLOR_CYAN << "Creating calamares-arch folder..." << COLOR_RESET << std::endl;
            std::string createCmd = "sudo mkdir -p " + calamaresFolder;
            execute_command(createCmd);
        }
        std::string sourceDir = currentDir + "/calamares-per-distro/arch";
        std::vector<std::string> zipFiles = {
            "calamares-arch.zip",
            "claudemods-arch.zip",
            "build-image-arch.zip"
        };
        for (const auto& zipFile : zipFiles) {
            std::string sourcePath = sourceDir + "/" + zipFile;
            if (fileExists(sourcePath)) {
                std::cout << COLOR_CYAN << "Extracting " << zipFile << "..." << COLOR_RESET << std::endl;
                std::string extractCmd = "sudo unzip -q " + sourcePath + " -d " + calamaresFolder + " >/dev/null 2>&1";
                execute_command(extractCmd);
                std::cout << COLOR_GREEN << "Done." << COLOR_RESET << std::endl;
            }
        }
        std::cout << COLOR_GREEN << "Extraction process completed." << COLOR_RESET << std::endl;
        return true;
    }

    void display_header() {
        std::cout << COLOR_RED;
        std::cout << "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░██████╗" << std::endl;
        std::cout << "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝" << std::endl;
        std::cout << "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░" << std::endl;
        std::cout << "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗" << std::endl;
        std::cout << "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝" << std::endl;
        std::cout << "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░" << std::endl;
        std::cout << COLOR_CYAN << "claudemods Arch Linux distribution installer iso method v1.01 06-07-2026" << COLOR_RESET << std::endl;
        std::cout << std::endl;
    }

    void setup_terminal() {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    void restore_terminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    int get_arrow_key() {
        int ch = getchar();
        if (ch == 27) {
            getchar();
            ch = getchar();
            return ch;
        }
        return ch;
    }

    int show_menu(const std::vector<std::string>& options, const std::string& title, int selected = 0) {
        setup_terminal();
        while (true) {
            system("clear");
            display_header();
            std::cout << COLOR_CYAN;
            std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║ " << std::left << std::setw(60) << title << "║" << std::endl;
            std::cout << "╠══════════════════════════════════════════════════════════════╣" << std::endl;
            for (int i = 0; i < options.size(); i++) {
                std::cout << "║ ";
                if (i == selected) {
                    std::cout << COLOR_GREEN << "> " << COLOR_RESET << COLOR_CYAN;
                } else {
                    std::cout << "  ";
                }
                std::string menu_line;
                if (title == "claudemods Arch Linux distribution installer") {
                    std::string setting_value;
                    switch(i) {
                        case 0: setting_value = target_drive.empty() ? "[Not Set]" : target_drive; break;
                        case 1: setting_value = new_username.empty() ? "[Not Set]" : new_username; break;
                        case 2: setting_value = root_password.empty() ? "[Not Set]" : root_password; break;
                        case 3: setting_value = user_password.empty() ? "[Not Set]" : user_password; break;
                        case 4: setting_value = timezone.empty() ? "[Not Set]" : timezone; break;
                        case 5: setting_value = keyboard_layout.empty() ? "[Not Set]" : keyboard_layout; break;
                        case 6: setting_value = selected_kernel.empty() ? "[Not Set]" : selected_kernel; break;
                        case 7: setting_value = " [Default GB]"; break;
                        case 8: setting_value = current_desktop_name.empty() ? "[Not Set]" : current_desktop_name; break;
                        case 9: setting_value = extra_packages.empty() ? "[Not Set]" : extra_packages; break;
                        default: setting_value = ""; break;
                    }
                    if (i == 0) {
                        menu_line = options[i];
                    } else if (i == 7) {
                        menu_line = options[i] + " " + setting_value;
                    } else {
                        menu_line = options[i] + " " + setting_value;
                    }
                } else {
                    menu_line = options[i];
                }
                if (menu_line.length() > 56) {
                    menu_line = menu_line.substr(0, 53) + "...";
                }
                std::cout << std::left << std::setw(56) << menu_line;
                std::cout << "║" << std::endl;
            }
            std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << COLOR_RESET;
            std::cout << COLOR_YELLOW << "Use ↑↓ arrows to navigate, Enter to select" << COLOR_RESET << std::endl;
            int key = get_arrow_key();
            if (key == 'A') {
                selected = (selected - 1 + options.size()) % options.size();
            } else if (key == 'B') {
                selected = (selected + 1) % options.size();
            } else if (key == 10) {
                restore_terminal();
                return selected;
            }
        }
    }

    std::string get_input(const std::string& prompt) {
        std::cout << COLOR_CYAN << prompt << COLOR_RESET;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    int execute_command(const std::string& cmd) {
        std::cout << COLOR_CYAN << std::flush;
        int status = system(cmd.c_str());
        std::cout << COLOR_RESET << std::flush;
        return status;
    }

    void display_current_settings() {
        std::cout << COLOR_YELLOW << "\nCurrent Settings:" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Target Drive: " << COLOR_RESET
                  << (target_drive.empty() ? "[Not Set]" : target_drive) << std::endl;
        std::cout << COLOR_CYAN << "Filesystem: " << COLOR_RESET
                  << (filesystem_type.empty() ? "[Not Set]" : filesystem_type) << std::endl;
        std::cout << COLOR_CYAN << "Username: " << COLOR_RESET
        << (new_username.empty() ? "[Not Set]" : new_username) << std::endl;
        std::cout << COLOR_CYAN << "Root Password: " << COLOR_RESET
        << (root_password.empty() ? "[Not Set]" : root_password) << std::endl;
        std::cout << COLOR_CYAN << "User Password: " << COLOR_RESET
        << (user_password.empty() ? "[Not Set]" : user_password) << std::endl;
        std::cout << COLOR_CYAN << "Timezone: " << COLOR_RESET
        << (timezone.empty() ? "[Not Set]" : timezone) << std::endl;
        std::cout << COLOR_CYAN << "Keyboard Layout: " << COLOR_RESET
        << (keyboard_layout.empty() ? "[Not Set]" : keyboard_layout) << std::endl;
        std::cout << COLOR_CYAN << "Kernel: " << COLOR_RESET
        << (selected_kernel.empty() ? "[Not Set]" : selected_kernel) << std::endl;
        std::cout << COLOR_CYAN << "Wireless Regulatory Domain: " << COLOR_RESET
        << "[Configure in needed-files/wireless-regdom]" << std::endl;
        std::cout << COLOR_CYAN << "Desktop Environment: " << COLOR_RESET
        << (current_desktop_name.empty() ? "[Not Set]" : current_desktop_name) << std::endl;
        std::cout << COLOR_CYAN << "Extra Packages: " << COLOR_RESET
        << (extra_packages.empty() ? "[Not Set]" : extra_packages) << std::endl;
        std::cout << std::endl;
    }

    void setup_bootloader_and_drive() {
        system("clear");
        display_header();
        std::cout << COLOR_CYAN << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║ " << std::left << std::setw(60) << "Setup Bootloader and Drive" << "║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Current Drive Settings:" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Target Drive: " << COLOR_RESET << (target_drive.empty() ? "[Not Set]" : target_drive) << std::endl;
        std::cout << COLOR_CYAN << "Filesystem: " << COLOR_RESET << (filesystem_type.empty() ? "[Not Set]" : filesystem_type) << std::endl;
        std::cout << std::endl;
        std::cout << COLOR_CYAN << "Available drives:" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW;
        std::string cmd = "lsblk -d -o NAME,SIZE,MODEL | grep -v 'loop\\|sr0\\|zram'";
        std::string result = exec_cmd(cmd.c_str());
        std::cout << "NAME        SIZE    MODEL" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::istringstream stream(result);
        std::string line;
        while (std::getline(stream, line)) {
            if (!line.empty()) {
                std::istringstream line_stream(line);
                std::string name, size, model;
                line_stream >> name;
                std::string size_str;
                if (line_stream >> size) { size_str = size; }
                getline(line_stream, model);
                size_t start = model.find_first_not_of(" \t");
                if (start != std::string::npos) { model = model.substr(start); }
                std::cout << "/dev/" << std::left << std::setw(10) << name << " " << std::setw(8) << size_str << model << std::endl;
            }
        }
        std::cout << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "WARNING: The selected drive will be COMPLETELY ERASED!" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Enter target drive (e.g., /dev/sda): " << COLOR_RESET;
        std::string drive_input;
        std::getline(std::cin, drive_input);
        if (drive_input.empty()) {
            std::cout << COLOR_YELLOW << "No drive selected. Keeping current setting." << COLOR_RESET << std::endl;
        } else if (!is_block_device(drive_input)) {
            std::cout << COLOR_RED << "Error: " << drive_input << " is not a valid block device!" << COLOR_RESET << std::endl;
        } else {
            target_drive = drive_input;
            std::cout << COLOR_GREEN << "Target drive set to: " << target_drive << COLOR_RESET << std::endl;
        }
        if (!target_drive.empty()) {
            std::cout << std::endl;
            std::cout << COLOR_CYAN << "Select filesystem type:" << COLOR_RESET << std::endl;
            std::vector<std::string> fs_options = {
                "Btrfs (with subvolumes, compression, snapshots support)",
                "Ext4 (standard filesystem)"
            };
            int fs_choice = show_menu(fs_options, "Select Filesystem Type");
            if (fs_choice == 0) {
                filesystem_type = "btrfs";
                std::cout << COLOR_GREEN << "Filesystem set to: Btrfs" << COLOR_RESET << std::endl;
                std::cout << COLOR_CYAN << "Btrfs subvolumes will be created: @, @home, @root, @srv, @cache, @tmp, @log" << COLOR_RESET << std::endl;
                std::cout << COLOR_CYAN << "Compression: zstd level 22" << COLOR_RESET << std::endl;
            } else {
                filesystem_type = "ext4";
                std::cout << COLOR_GREEN << "Filesystem set to: Ext4" << COLOR_RESET << std::endl;
            }
        }
        if (!target_drive.empty() && !filesystem_type.empty()) {
            std::cout << std::endl;
            std::cout << COLOR_ORANGE << "═══════════════════════════════════════" << COLOR_RESET << std::endl;
            std::cout << COLOR_ORANGE << "  Drive Configuration Summary:" << COLOR_RESET << std::endl;
            std::cout << COLOR_ORANGE << "  Drive: " << target_drive << COLOR_RESET << std::endl;
            std::cout << COLOR_ORANGE << "  Partition 1: " << target_drive << "1 (EFI - FAT32, 550MB)" << COLOR_RESET << std::endl;
            std::cout << COLOR_ORANGE << "  Partition 2: " << target_drive << "2 (Root - " << (filesystem_type == "btrfs" ? "Btrfs" : "Ext4") << ")" << COLOR_RESET << std::endl;
            std::cout << COLOR_ORANGE << "  Bootloader: GRUB (UEFI)" << COLOR_RESET << std::endl;
            std::cout << COLOR_ORANGE << "═══════════════════════════════════════" << COLOR_RESET << std::endl;
            std::cout << std::endl;
            std::cout << COLOR_RED << "WARNING: ALL DATA ON " << target_drive << " WILL BE DESTROYED!" << COLOR_RESET << std::endl;
        }
        saveConfiguration();
    }

    void prepare_target_partitions() {
        std::cout << COLOR_CYAN << "Preparing target partitions on " << target_drive << "..." << COLOR_RESET << std::endl;
        execute_command("sudo umount -f " + target_drive + "* 2>/dev/null || true");
        execute_command("sudo wipefs -a " + target_drive);
        execute_command("sudo parted -s " + target_drive + " mklabel gpt");
        execute_command("sudo parted -s " + target_drive + " mkpart primary fat32 1MiB 551MiB");
        execute_command("sudo parted -s " + target_drive + " mkpart primary " + filesystem_type + " 551MiB 100%");
        execute_command("sudo parted -s " + target_drive + " set 1 esp on");
        execute_command("sudo partprobe " + target_drive);
        sleep(2);
        std::string efi_part = target_drive + "1";
        std::string root_part = target_drive + "2";
        if (!is_block_device(efi_part) || !is_block_device(root_part)) {
            std::cerr << COLOR_RED << "Error: Failed to create partitions" << COLOR_RESET << std::endl;
            exit(1);
        }
        execute_command("sudo mkfs.vfat -F32 " + efi_part);
        if (filesystem_type == "btrfs") {
            execute_command("sudo mkfs.btrfs -f -L ROOT " + root_part);
        } else {
            execute_command("sudo mkfs.ext4 -F -L ROOT " + root_part);
        }
    }

    void setup_btrfs_subvolumes() {
        std::string root_part = target_drive + "2";
        std::cout << COLOR_CYAN << "Setting up Btrfs subvolumes with zstd:22 compression..." << COLOR_RESET << std::endl;
        execute_command("sudo mount " + root_part + " /mnt");
        execute_command("sudo btrfs subvolume create /mnt/@");
        execute_command("sudo btrfs subvolume create /mnt/@home");
        execute_command("sudo btrfs subvolume create /mnt/@root");
        execute_command("sudo btrfs subvolume create /mnt/@srv");
        execute_command("sudo btrfs subvolume create /mnt/@cache");
        execute_command("sudo btrfs subvolume create /mnt/@tmp");
        execute_command("sudo btrfs subvolume create /mnt/@log");
        execute_command("sudo mkdir -p /mnt/@/var/lib");
        execute_command("sudo btrfs subvolume create /mnt/@/var/lib/portables");
        execute_command("sudo btrfs subvolume create /mnt/@/var/lib/machines");
        execute_command("sudo umount /mnt");
        execute_command("sudo mount -o subvol=@,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt");
        execute_command("sudo mkdir -p /mnt/{home,root,srv,tmp,var/{cache,log},var/lib/{portables,machines},boot/efi}");
        execute_command("sudo mount -o subvol=@home,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/home");
        execute_command("sudo mount -o subvol=@root,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/root");
        execute_command("sudo mount -o subvol=@srv,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/srv");
        execute_command("sudo mount -o subvol=@cache,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/var/cache");
        execute_command("sudo mount -o subvol=@tmp,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/tmp");
        execute_command("sudo mount -o subvol=@log,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/var/log");
        execute_command("sudo mount -o subvol=@/var/lib/portables,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/var/lib/portables");
        execute_command("sudo mount -o subvol=@/var/lib/machines,compress=zstd:22,compress-force=zstd:22 " + root_part + " /mnt/var/lib/machines");
    }

    void setup_ext4_filesystem() {
        std::string root_part = target_drive + "2";
        std::cout << COLOR_CYAN << "Setting up Ext4 filesystem..." << COLOR_RESET << std::endl;
        execute_command("sudo mount " + root_part + " /mnt");
        execute_command("sudo mkdir -p /mnt/{home,boot/efi,etc,usr,var,proc,sys,dev,tmp,run}");
    }

    void install_grub() {
        std::cout << COLOR_CYAN << "Installing GRUB bootloader..." << COLOR_RESET << std::endl;
        std::string efi_part = target_drive + "1";
        execute_command("sudo mount " + efi_part + " /mnt/boot/efi");
        execute_command("sudo mount --bind /sys/firmware/efi/efivars /mnt/sys/firmware/efi/efivars 2>/dev/null || true");
        execute_command("sudo mount --bind /dev /mnt/dev");
        execute_command("sudo mount --bind /dev/pts /mnt/dev/pts");
        execute_command("sudo mount --bind /proc /mnt/proc");
        execute_command("sudo mount --bind /sys /mnt/sys");
        execute_command("sudo mount --bind /run /mnt/run");
        if (filesystem_type == "btrfs") {
            execute_command("sudo touch /mnt/etc/fstab");
            execute_command("sudo cp btrfsfstabcompressed.sh /mnt/opt/btrfsfstabcompressed.sh");
            execute_command("sudo chroot /mnt /bin/bash -c \""
                "modprobe efivarfs 2>/dev/null || true; "
                "mount -t efivarfs efivarfs /sys/firmware/efi/efivars 2>/dev/null || true; "
                "grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=GRUB --recheck; "
                "grub-mkconfig -o /boot/grub/grub.cfg; "
                "./opt/btrfsfstabcompressed.sh 2>/dev/null; "
                "rm -rf /opt/btrfsfstabcompressed.sh 2>/dev/null; "
                "pacman -Scc --noconfirm; "
                "mkinitcpio -P\"");
        } else {
            execute_command("sudo chroot /mnt /bin/bash -c \""
                "modprobe efivarfs 2>/dev/null || true; "
                "mount -t efivarfs efivarfs /sys/firmware/efi/efivars 2>/dev/null || true; "
                "genfstab -U / >> /etc/fstab; "
                "grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=GRUB --recheck; "
                "grub-mkconfig -o /boot/grub/grub.cfg; "
                "pacman -Scc --noconfirm; "
                "mkinitcpio -P\"");
        }
    }

    void unmount_target() {
        std::cout << COLOR_CYAN << "Unmounting target filesystems..." << COLOR_RESET << std::endl;
        execute_command("sudo umount -l /mnt 2>/dev/null || true");
    }

    void post_install_menu() {
        bool menu_running = true;
        while (menu_running) {
            std::cout << "\n" << COLOR_CYAN;
            std::cout << "╔══════════════════════════════════════════════╗" << std::endl;
            std::cout << "║           INSTALLATION COMPLETE              ║" << std::endl;
            std::cout << "╠══════════════════════════════════════════════╣" << std::endl;
            std::cout << "║  1. Reboot now                              ║" << std::endl;
            std::cout << "║  2. Exit to shell                           ║" << std::endl;
            std::cout << "╚══════════════════════════════════════════════╝" << std::endl;
            std::cout << COLOR_RESET;
            std::cout << COLOR_CYAN << "Choose an option (1-2): " << COLOR_RESET;
            std::string choice;
            std::getline(std::cin, choice);
            if (choice == "1") {
                std::cout << COLOR_CYAN << "Unmounting and rebooting..." << COLOR_RESET << std::endl;
                unmount_target();
                execute_command("sudo reboot");
                menu_running = false;
            }
            else if (choice == "2") {
                std::cout << COLOR_CYAN << "Exiting to shell. System is still mounted at /mnt" << COLOR_RESET << std::endl;
                menu_running = false;
            }
            else {
                std::cout << COLOR_RED << "Invalid option. Please choose 1 or 2." << COLOR_RESET << std::endl;
            }
        }
    }

    bool setup_installation_environment() {
        std::string target_folder = getFullTargetPath();
        std::string currentDir = getCurrentDir();
        std::cout << COLOR_CYAN << "Creating target directory: " << target_folder << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder);
        struct stat info;
        if (stat(target_folder.c_str(), &info) != 0) {
            std::cerr << COLOR_RED << "Failed to create target directory: " << target_folder << COLOR_RESET << std::endl;
            return false;
        }
        if (!(info.st_mode & S_IFDIR)) {
            std::cerr << COLOR_RED << "Target path is not a directory: " << target_folder << COLOR_RESET << std::endl;
            return false;
        }
        std::cout << COLOR_GREEN << "Target directory created successfully!" << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder + "/usr/");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib/initcpio/");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib/initcpio/udev/");
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules " + target_folder + "/usr/lib/initcpio/udev/11-dm-initramfs.rules");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules /usr/lib/initcpio/udev/11-dm-initramfs.rules");
        execute_command("sudo cp -r " + currentDir + "/needed-files/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");
        execute_command("sudo cp -r " + currentDir + "/needed-files/pacman-arch.conf /etc/pacman.conf");
        execute_command("sudo pacman -Sy");
        execute_command("sudo pacman -S archlinux-keyring");
        execute_command("sudo pacman-key --populate");
        execute_command("sudo pacman-key --init");
        return true;
    }

    bool install_base_packages(const std::string& desktop_packages, const std::string& display_manager = "") {
        std::string target_folder = getFullTargetPath();
        std::string currentDir = getCurrentDir();
        std::string packages = "base base-devel calamares-fix dosfstools mtools rsync btrfs-progs f2fs-tools xfsprogs parted linux-firmware grub efibootmgr os-prober sudo arch-install-scripts mkinitcpio vim nano bash-completion systemd networkmanager " + selected_kernel;
        if (!desktop_packages.empty()) { packages += " " + desktop_packages; }
        if (!display_manager.empty()) { packages += " " + display_manager; }
        if (!extra_packages.empty()) { packages += " " + extra_packages; }
        std::cout << COLOR_CYAN << "Installing packages with pacstrap..." << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Package list: " << packages << COLOR_RESET << std::endl;
        execute_command("sudo pacstrap " + target_folder + " " + packages);
        std::string test_bin = target_folder + "/bin/bash";
        struct stat info;
        if (stat(test_bin.c_str(), &info) != 0) {
            std::cerr << COLOR_RED << "pacstrap failed! /bin/bash not found in target." << COLOR_RESET << std::endl;
            return false;
        }
        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");
        return true;
    }

    void install_calamares() {
        std::string target_folder = getFullTargetPath();
        std::string currentDir = getCurrentDir();
        std::cout << COLOR_CYAN << "Installing Extras..." << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Desktop");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Documents");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Downloads");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Music");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Pictures");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Videos");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Public");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Templates");
        std::cout << COLOR_CYAN << "Cleaning pacman cache..." << COLOR_RESET << std::endl;
        std::string cache_clean_cmd = "sudo rm -rf " + target_folder + "/var/cache/pacman/pkg/*";
        if (execute_command(cache_clean_cmd) == 0) {
            std::cout << COLOR_GREEN << "Pacman cache cleaned successfully!" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "Failed to clean pacman cache!" << COLOR_RESET << std::endl;
        }
        execute_command("sudo cp -r " + currentDir + "/needed-files/wireless-regdom " + target_folder + "/etc/conf.d/wireless-regdom");
        std::cout << COLOR_GREEN << "installation completed!" << COLOR_RESET << std::endl;
    }

    void enable_services(const std::string& display_manager_service) {
        std::string target_folder = getFullTargetPath();
        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");
        if (!display_manager_service.empty()) {
            execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable " + display_manager_service + "\"");
        }
        apply_timezone_keyboard_settings();
        create_user();
        install_calamares();
    }

    void set_username() {
        new_username = get_input("Enter username: ");
        saveConfiguration();
    }

    void set_root_password() {
        root_password = get_input("Enter root password: ");
        saveConfiguration();
    }

    void set_user_password() {
        user_password = get_input("Enter user password: ");
        saveConfiguration();
    }

    void set_timezone() {
        std::vector<std::string> timezone_options = {
            "America/New_York (US English)",
            "Europe/London (UK English)",
            "Europe/Berlin (German)",
            "Europe/Paris (French)",
            "Europe/Madrid (Spanish)",
            "Europe/Rome (Italian)",
            "Asia/Tokyo (Japanese)",
            "Other (manual entry)"
        };
        int timezone_choice = show_menu(timezone_options, "Select Timezone");
        switch(timezone_choice) {
            case 0: timezone = "America/New_York"; break;
            case 1: timezone = "Europe/London"; break;
            case 2: timezone = "Europe/Berlin"; break;
            case 3: timezone = "Europe/Paris"; break;
            case 4: timezone = "Europe/Madrid"; break;
            case 5: timezone = "Europe/Rome"; break;
            case 6: timezone = "Asia/Tokyo"; break;
            case 7: timezone = get_input("Enter timezone (e.g., Europe/Berlin): "); break;
        }
        saveConfiguration();
    }

    void set_keyboard_layout() {
        std::vector<std::string> keyboard_options = {
            "us (US English)",
            "uk (UK English)",
            "de (German)",
            "fr (French)",
            "es (Spanish)",
            "it (Italian)",
            "jp (Japanese)",
            "Other (manual entry)"
        };
        int keyboard_choice = show_menu(keyboard_options, "Select Keyboard Layout");
        switch(keyboard_choice) {
            case 0: keyboard_layout = "us"; break;
            case 1: keyboard_layout = "uk"; break;
            case 2: keyboard_layout = "de"; break;
            case 3: keyboard_layout = "fr"; break;
            case 4: keyboard_layout = "es"; break;
            case 5: keyboard_layout = "it"; break;
            case 6: keyboard_layout = "jp"; break;
            case 7: keyboard_layout = get_input("Enter keyboard layout (e.g., br, ru, pt): "); break;
        }
        saveConfiguration();
    }

    void set_kernel() {
        std::vector<std::string> kernel_options = {
            "linux (Default, Stable)",
            "linux-lts (Long Term Support)",
            "linux-zen (Performance Optimized)",
            "linux-hardened (Security Hardened)"
        };
        int kernel_choice = show_menu(kernel_options, "Select Arch Linux Kernel");
        switch(kernel_choice) {
            case 0: selected_kernel = "linux"; break;
            case 1: selected_kernel = "linux-lts"; break;
            case 2: selected_kernel = "linux-zen"; break;
            case 3: selected_kernel = "linux-hardened"; break;
        }
        saveConfiguration();
    }

    void set_wireless_regdom() {
        std::string currentDir = getCurrentDir();
        std::string wireless_regdom_file = currentDir + "/needed-files/wireless-regdom";
        std::cout << COLOR_CYAN << "Opening wireless regulatory domain configuration file..." << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "File location: " << wireless_regdom_file << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Press Enter to open the file in nano editor..." << COLOR_RESET << std::endl;
        std::cin.get();
        std::string nano_cmd = "nano " + wireless_regdom_file;
        int result = execute_command(nano_cmd);
        if (result == 0) {
            std::cout << COLOR_GREEN << "Wireless regulatory domain configuration saved." << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "Failed to edit wireless regulatory domain file." << COLOR_RESET << std::endl;
            std::cout << COLOR_YELLOW << "You can manually edit the file at: " << wireless_regdom_file << COLOR_RESET << std::endl;
        }
    }

    void set_extra_packages() {
        std::cout << COLOR_CYAN << "Enter additional packages to install (space-separated):" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Examples: vim git htop curl wget firefox" << COLOR_RESET << std::endl;
        extra_packages = get_input("Extra packages: ");
        saveConfiguration();
    }

    bool check_settings_configured() {
        if (target_drive.empty()) {
            std::cout << COLOR_RED << "Error: Target drive not set! Use 'Setup Bootloader and Drive' first." << COLOR_RESET << std::endl;
            return false;
        }
        if (filesystem_type.empty()) {
            std::cout << COLOR_RED << "Error: Filesystem type not set! Use 'Setup Bootloader and Drive' first." << COLOR_RESET << std::endl;
            return false;
        }
        if (new_username.empty()) {
            std::cout << COLOR_RED << "Error: Username not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (root_password.empty()) {
            std::cout << COLOR_RED << "Error: Root password not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (user_password.empty()) {
            std::cout << COLOR_RED << "Error: User password not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (timezone.empty()) {
            std::cout << COLOR_RED << "Error: Timezone not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (keyboard_layout.empty()) {
            std::cout << COLOR_RED << "Error: Keyboard layout not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (selected_kernel.empty()) {
            std::cout << COLOR_RED << "Error: Kernel not selected!" << COLOR_RESET << std::endl;
            return false;
        }
        if (current_desktop_name.empty()) {
            std::cout << COLOR_RED << "Error: Desktop environment not selected!" << COLOR_RESET << std::endl;
            return false;
        }
        return true;
    }

    void mount_system_dirs() {
        execute_command("sudo mkdir -p /mnt/dev");
        execute_command("sudo mkdir -p /mnt/dev/pts");
        execute_command("sudo mkdir -p /mnt/proc");
        execute_command("sudo mkdir -p /mnt/sys");
        execute_command("sudo mkdir -p /mnt/run");
        execute_command("sudo mkdir -p /mnt/etc");
        execute_command("sudo mount --bind /dev /mnt/dev");
        execute_command("sudo mount --bind /dev/pts /mnt/dev/pts");
        execute_command("sudo mount --bind /proc /mnt/proc");
        execute_command("sudo mount --bind /sys /mnt/sys");
        execute_command("sudo mount --bind /run /mnt/run");
    }

    void unmount_system_dirs() {
        execute_command("sudo umount /mnt/dev/pts");
        execute_command("sudo umount /mnt/dev");
        execute_command("sudo umount /mnt/proc");
        execute_command("sudo umount /mnt/sys");
        execute_command("sudo umount /mnt/run");
    }

    void create_user() {
        execute_command("sudo chroot /mnt /bin/bash -c \"useradd -m -G wheel -s /bin/bash " + new_username + "\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo 'root:" + root_password + "' | chpasswd\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo '" + new_username + ":" + user_password + "' | chpasswd\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\"");
    }

    void apply_timezone_keyboard_settings() {
        std::cout << COLOR_CYAN << "Setting timezone to: " << timezone << COLOR_RESET << std::endl;
        execute_command("sudo chroot /mnt /bin/bash -c \"ln -sf /usr/share/zoneinfo/" + timezone + " /etc/localtime\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"hwclock --systohc\"");
        std::cout << COLOR_CYAN << "Setting keyboard layout to: " << keyboard_layout << COLOR_RESET << std::endl;
        execute_command("sudo chroot /mnt /bin/bash -c \"echo 'KEYMAP=" + keyboard_layout + "' > /etc/vconsole.conf\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"locale-gen\"");
    }

    void start_installation() {
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }
        prepare_target_partitions();
        if (filesystem_type == "btrfs") { setup_btrfs_subvolumes(); } else { setup_ext4_filesystem(); }
        std::cout << COLOR_CYAN << "Starting installation with selected desktop: " << current_desktop_name << COLOR_RESET << std::endl;
        if (current_desktop_name == "Arch-TTY-Grub") { install_arch_tty_grub(); }
        else if (current_desktop_name == "GNOME-Desktop") { install_gnome_desktop(); }
        else if (current_desktop_name == "KDE-Plasma") { install_kde_plasma(); }
        else if (current_desktop_name == "XFCE-Desktop") { install_xfce_desktop(); }
        else if (current_desktop_name == "LXQt-Desktop") { install_lxqt_desktop(); }
        else if (current_desktop_name == "Cinnamon-Desktop") { install_cinnamon_desktop(); }
        else if (current_desktop_name == "MATE-Desktop") { install_mate_desktop(); }
        else if (current_desktop_name == "Budgie-Desktop") { install_budgie_desktop(); }
        else if (current_desktop_name == "i3-WM") { install_i3_wm(); }
        else if (current_desktop_name == "Sway-WM") { install_sway_wm(); }
        else if (current_desktop_name == "Hyprland-WM") { install_hyprland_wm(); }
        else { std::cout << COLOR_RED << "Unknown desktop environment: " << current_desktop_name << COLOR_RESET << std::endl; }
    }

    void install_arch_tty_grub() {
        std::cout << COLOR_CYAN << "Installing Arch Linux TTY Grub to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("", "")) return;
        enable_services("");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-TTY-Grub installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_gnome_desktop() {
        std::cout << COLOR_CYAN << "Installing GNOME Desktop to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("gnome gnome-extra", "gdm")) return;
        enable_services("gdm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-GNOME-Desktop installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_kde_plasma() {
        std::cout << COLOR_CYAN << "Installing KDE Plasma to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("plasma dolphin ark konsole kate gwenview ksystemlog", "sddm")) return;
        enable_services("sddm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-KDE-Plasma installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_xfce_desktop() {
        std::cout << COLOR_CYAN << "Installing XFCE Desktop to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("xfce4 xfce4-goodies lightdm lightdm-gtk-greeter", "lightdm")) return;
        enable_services("lightdm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-XFCE-Desktop installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_lxqt_desktop() {
        std::cout << COLOR_CYAN << "Installing LXQt Desktop to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("lxqt xdg-utils sddm", "sddm")) return;
        enable_services("sddm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-LXQt-Desktop installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_cinnamon_desktop() {
        std::cout << COLOR_CYAN << "Installing Cinnamon Desktop to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("cinnamon lightdm lightdm-gtk-greeter", "lightdm")) return;
        enable_services("lightdm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-Cinnamon-Desktop installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_mate_desktop() {
        std::cout << COLOR_CYAN << "Installing MATE Desktop to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("mate mate-extra lightdm lightdm-gtk-greeter", "lightdm")) return;
        enable_services("lightdm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-MATE-Desktop installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_budgie_desktop() {
        std::cout << COLOR_CYAN << "Installing Budgie Desktop to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("budgie-desktop gnome-control-center lightdm lightdm-gtk-greeter", "lightdm")) return;
        enable_services("lightdm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-Budgie-Desktop installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_i3_wm() {
        std::cout << COLOR_CYAN << "Installing i3 Window Manager to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("i3-wm i3status i3lock dmenu rxvt-unicode lightdm lightdm-gtk-greeter", "lightdm")) return;
        enable_services("lightdm");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-i3-WM installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_sway_wm() {
        std::cout << COLOR_CYAN << "Installing Sway Window Manager to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("sway waybar wofi foot", "")) return;
        enable_services("");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-Sway-WM installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_hyprland_wm() {
        std::cout << COLOR_CYAN << "Installing Hyprland Window Manager to " << target_drive << "..." << COLOR_RESET << std::endl;
        if (!setup_installation_environment()) return;
        if (!install_base_packages("hyprland waybar rofi foot", "")) return;
        enable_services("");
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "Arch-Hyprland-WM installation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void show_desktop_selection() {
        std::vector<std::string> desktop_options = {
            "Arch TTY Grub (Terminal Only)",
            "GNOME Desktop",
            "KDE Plasma",
            "XFCE Desktop",
            "LXQt Desktop",
            "Cinnamon Desktop",
            "MATE Desktop",
            "Budgie Desktop",
            "i3 (tiling WM)",
            "Sway (Wayland tiling)",
            "Hyprland (Wayland)",
            "Back to Main Menu"
        };
        int selected = 0;
        while (true) {
            selected = show_menu(desktop_options, "Select Arch Linux Desktop Environment", selected);
            switch(selected) {
                case 0: current_desktop_name = "Arch-TTY-Grub"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: Arch TTY Grub" << COLOR_RESET << std::endl; return;
                case 1: current_desktop_name = "GNOME-Desktop"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: GNOME Desktop" << COLOR_RESET << std::endl; return;
                case 2: current_desktop_name = "KDE-Plasma"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: KDE Plasma" << COLOR_RESET << std::endl; return;
                case 3: current_desktop_name = "XFCE-Desktop"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: XFCE Desktop" << COLOR_RESET << std::endl; return;
                case 4: current_desktop_name = "LXQt-Desktop"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: LXQt Desktop" << COLOR_RESET << std::endl; return;
                case 5: current_desktop_name = "Cinnamon-Desktop"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: Cinnamon Desktop" << COLOR_RESET << std::endl; return;
                case 6: current_desktop_name = "MATE-Desktop"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: MATE Desktop" << COLOR_RESET << std::endl; return;
                case 7: current_desktop_name = "Budgie-Desktop"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: Budgie Desktop" << COLOR_RESET << std::endl; return;
                case 8: current_desktop_name = "i3-WM"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: i3 Window Manager" << COLOR_RESET << std::endl; return;
                case 9: current_desktop_name = "Sway-WM"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: Sway Window Manager" << COLOR_RESET << std::endl; return;
                case 10: current_desktop_name = "Hyprland-WM"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Desktop environment set to: Hyprland Window Manager" << COLOR_RESET << std::endl; return;
                case 11: return;
            }
        }
    }

    void show_main_menu() {
        std::vector<std::string> main_options = {
            "Setup Bootloader and Drive: ",
            "Set Username",
            "Set Root Password",
            "Set User Password",
            "Set Timezone",
            "Set Keyboard Layout",
            "Set Kernel",
            "Set Wireless Regdom",
            "Select Desktop Environment",
            "Install Extra Packages",
            "Start Installation",
            "Exit"
        };
        int selected = 0;
        while (true) {
            system("clear");
            display_header();
            display_current_settings();
            selected = show_menu(main_options, "claudemods Arch Linux distribution installer", selected);
            switch(selected) {
                case 0: setup_bootloader_and_drive(); break;
                case 1: set_username(); break;
                case 2: set_root_password(); break;
                case 3: set_user_password(); break;
                case 4: set_timezone(); break;
                case 5: set_keyboard_layout(); break;
                case 6: set_kernel(); break;
                case 7: set_wireless_regdom(); break;
                case 8: show_desktop_selection(); break;
                case 9: set_extra_packages(); break;
                case 10: start_installation(); break;
                case 11: std::cout << COLOR_GREEN << "Exiting. Goodbye!" << COLOR_RESET << std::endl; return;
            }
            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

public:
    void run() {
        loadConfiguration();
        if (!extractRequiredFiles()) {
            std::cerr << COLOR_RED << "Failed to extract required files. Cannot continue." << COLOR_RESET << std::endl;
            return;
        }
        display_header();
        show_main_menu();
    }
};

#endif // ARCH_H
