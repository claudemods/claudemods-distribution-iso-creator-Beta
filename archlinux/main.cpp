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

// Include your resource manager
#include "resources.h"

// Color definitions
const std::string COLOR_CYAN = "\033[38;2;0;255;255m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_ORANGE = "\033[38;5;208m";
const std::string COLOR_PURPLE = "\033[38;5;93m";
const std::string COLOR_RESET = "\033[0m";

class ArchDesktopInstaller {
private:
    std::string new_username;
    std::string root_password;
    std::string user_password;
    std::string timezone;
    std::string keyboard_layout;
    std::string current_desktop_name;
    std::string selected_kernel;

    // Terminal control for arrow keys
    struct termios oldt, newt;

    // Get current working directory
    std::string getCurrentDir() {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            return std::string(cwd);
        }
        return ".";
    }

    // Get target folder path (always in current directory)
    std::string getTargetFolder() {
        return getCurrentDir() + "/arch-desktop-install";
    }

    // ADD ZIP EXTRACTION FUNCTION
    bool extractRequiredFiles() {
        std::cout << COLOR_CYAN << "Extracting required files..." << COLOR_RESET << std::endl;

        // Use your existing resource manager functions
        if (!ResourceManager::extractEmbeddedZip("")) {
            std::cerr << COLOR_RED << "Failed to extract embedded ZIP files!" << COLOR_RESET << std::endl;
            return false;
        }

        if (!ResourceManager::extractCalamaresResources("")) {
            std::cerr << COLOR_RED << "Failed to extract Calamares resources!" << COLOR_RESET << std::endl;
            return false;
        }

        std::cout << COLOR_GREEN << "All required files extracted successfully!" << COLOR_RESET << std::endl;
        return true;
    }

    void display_header() {
        std::cout << COLOR_RED;
        std::cout << "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗" << std::endl;
        std::cout << "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝" << std::endl;
        std::cout << "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░" << std::endl;
        std::cout << "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗" << std::endl;
        std::cout << "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝" << std::endl;
        std::cout << "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░" << std::endl;
        std::cout << COLOR_CYAN << "claudemods arch distribution iso creator v1.0 17-11-2025" << COLOR_RESET << std::endl;
        std::cout << std::endl;
    }

    // Function to setup terminal for arrow key reading
    void setup_terminal() {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    // Function to restore terminal
    void restore_terminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    // Function to get arrow key input
    int get_arrow_key() {
        int ch = getchar();
        if (ch == 27) { // ESC
            getchar(); // Skip [
            ch = getchar(); // Actual key
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

                // Create the menu line with proper formatting
                std::string menu_line;
                if (title == "claudemods arch distribution iso creator") {
                    std::string setting_value;
                    switch(i) {
                        case 0: setting_value = getTargetFolder(); break;
                        case 1: setting_value = new_username.empty() ? "[Not Set]" : new_username; break;
                        case 2: setting_value = root_password.empty() ? "[Not Set]" : root_password; break;
                        case 3: setting_value = user_password.empty() ? "[Not Set]" : user_password; break;
                        case 4: setting_value = timezone.empty() ? "[Not Set]" : timezone; break;
                        case 5: setting_value = keyboard_layout.empty() ? "[Not Set]" : keyboard_layout; break;
                        case 6: setting_value = selected_kernel.empty() ? "[Not Set]" : selected_kernel; break;
                        case 7: setting_value = current_desktop_name.empty() ? "[Not Set]" : current_desktop_name; break;
                        default: setting_value = ""; break;
                    }

                    // Build the complete line with menu item and setting
                    menu_line = options[i] + " " + setting_value;
                } else {
                    menu_line = options[i];
                }

                // Truncate if too long to fit in the box
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

    // Function to get text input with prompt
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

    // FIXED: Function to display current settings on main menu
    void display_current_settings() {
        std::cout << COLOR_YELLOW << "\nCurrent Settings:" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Installation Path: " << COLOR_RESET << getTargetFolder() << std::endl;
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
        std::cout << COLOR_CYAN << "Desktop Environment: " << COLOR_RESET
                  << (current_desktop_name.empty() ? "[Not Set]" : current_desktop_name) << std::endl;
        std::cout << std::endl;
    }

    // UPDATED: Function to create squashfs image after installation with additional steps
void create_squashfs_image(const std::string& distro_name) {
    std::cout << COLOR_CYAN << "Creating squashfs image..." << COLOR_RESET << std::endl;

    std::string currentDir = getCurrentDir();
    std::string target_folder = getTargetFolder();

    // NEW: Clean pacman cache before creating squashfs
    std::cout << COLOR_CYAN << "Cleaning pacman cache..." << COLOR_RESET << std::endl;
    std::string cache_clean_cmd = "sudo rm -rf " + target_folder + "/var/cache/pacman/pkg/*";
    if (execute_command(cache_clean_cmd) == 0) {
        std::cout << COLOR_GREEN << "Pacman cache cleaned successfully!" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "Failed to clean pacman cache!" << COLOR_RESET << std::endl;
    }

    // Remove mtab before squashfs creation
    execute_command("sudo rm -rf " + target_folder + "/etc/mtab");

    std::string squashfs_cmd = "sudo mksquashfs " + target_folder + " " + currentDir + "/build-image-arch-img/LiveOS/rootfs.img -noappend -comp xz -b 256K -Xbcj x86";

    std::cout << COLOR_CYAN << "Executing: " << squashfs_cmd << COLOR_RESET << std::endl;

    if (execute_command(squashfs_cmd) == 0) {
        std::cout << COLOR_GREEN << "Squashfs image created successfully!" << COLOR_RESET << std::endl;

            // Clean up target folder after successful squashfs creation
            std::cout << COLOR_CYAN << "Cleaning up target folder..." << COLOR_RESET << std::endl;
            std::string cleanup_cmd = "sudo rm -rf " + target_folder;
            if (execute_command(cleanup_cmd) == 0) {
                std::cout << COLOR_GREEN << "Target folder deleted successfully: " << target_folder << COLOR_RESET << std::endl;
            }

            // Copy kernel image
            std::cout << COLOR_CYAN << "Searching for kernel images..." << COLOR_RESET << std::endl;
            std::vector<std::string> kernel_files;
            std::string find_cmd = "find /boot -name 'vmlinuz-*' -type f 2>/dev/null | sort";
            FILE* pipe = popen(find_cmd.c_str(), "r");
            if (pipe) {
                char buffer[256];
                while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    std::string file_path = buffer;
                    file_path.erase(std::remove(file_path.begin(), file_path.end(), '\n'), file_path.end());
                    if (!file_path.empty()) kernel_files.push_back(file_path);
                }
                pclose(pipe);
            }

            if (!kernel_files.empty()) {
                std::string copy_cmd = "sudo cp " + kernel_files[0] + " " + currentDir + "/build-image-arch-img/boot/vmlinuz-x86_64";
                if (execute_command(copy_cmd) == 0) {
                    std::cout << COLOR_GREEN << "Kernel copied successfully!" << COLOR_RESET << std::endl;
                }
            }

            // Generate initramfs
            std::cout << COLOR_CYAN << "Generating initramfs..." << COLOR_RESET << std::endl;
            std::string initramfs_cmd = "cd " + currentDir + "/build-image-arch-img && sudo mkinitcpio -c mkinitcpio.conf -g " + currentDir + "/build-image-arch-img/boot/initramfs-x86_64.img";
            if (execute_command(initramfs_cmd) == 0) {
                std::cout << COLOR_GREEN << "Initramfs generated successfully!" << COLOR_RESET << std::endl;
                create_iso_image(desktop_name);
            }
        } else {
            std::cout << COLOR_RED << "Failed to create squashfs image!" << COLOR_RESET << std::endl;
        }
    }

    // Create ISO image with XORRISO
    void create_iso_image(const std::string& desktop_name) {
        std::cout << COLOR_CYAN << "Creating ISO image with XORRISO..." << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // Build the XORRISO command
        std::string xorriso_cmd = "sudo xorriso -as mkisofs "
        "--modification-date=\"$(date +%Y%m%d%H%M%S00)\" "
        "--protective-msdos-label "
        "-volid \"2025\" "
        "-appid \"Arch Linux Desktop Live CD\" "
        "-publisher \"Arch Linux Installer\" "
        "-preparer \"Prepared by user\" "
        "-r -graft-points -no-pad "
        "--sort-weight 0 / "
        "--sort-weight 1 /boot "
        "--grub2-mbr " + currentDir + "/build-image-arch-img/boot/grub/i386-pc/boot_hybrid.img "
        "-partition_offset 16 "
        "-b boot/grub/i386-pc/eltorito.img "
        "-c boot.catalog "
        "-no-emul-boot -boot-load-size 4 -boot-info-table --grub2-boot-info "
        "-eltorito-alt-boot "
        "-append_partition 2 0xef " + currentDir + "/build-image-arch-img/boot/efi.img "
        "-e --interval:appended_partition_2:all:: "
        "-no-emul-boot "
        "-iso-level 3 "
        "-o \"" + currentDir + "/" + desktop_name + ".iso\" " +
        currentDir + "/build-image-arch-img/";

        std::cout << COLOR_CYAN << "Executing: " << xorriso_cmd << COLOR_RESET << std::endl;

        if (execute_command(xorriso_cmd) == 0) {
            std::cout << COLOR_GREEN << "ISO image created successfully: " << desktop_name + ".iso" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "Failed to create ISO image!" << COLOR_RESET << std::endl;
        }
    }

    // Set username
    void set_username() {
        new_username = get_input("Enter username: ");
    }

    // Set root password
    void set_root_password() {
        root_password = get_input("Enter root password: ");
    }

    // Set user password
    void set_user_password() {
        user_password = get_input("Enter user password: ");
    }

    // Set timezone
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
    }

    // Set keyboard layout
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
    }

    // Set kernel
    void set_kernel() {
        std::vector<std::string> kernel_options = {
            "linux (Standard)",
            "linux-lts (Long Term Support)",
            "linux-zen (Tuned for desktop performance)",
            "linux-hardened (Security-focused)"
        };

        int kernel_choice = show_menu(kernel_options, "Select Kernel");
        switch(kernel_choice) {
            case 0: selected_kernel = "linux"; break;
            case 1: selected_kernel = "linux-lts"; break;
            case 2: selected_kernel = "linux-zen"; break;
            case 3: selected_kernel = "linux-hardened"; break;
        }
    }

    // Check if all required settings are configured
    bool check_settings_configured() {
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
        return true;
    }

    void mount_system_dirs() {
        std::string target_folder = getTargetFolder();
        execute_command("sudo mkdir -p " + target_folder + "/dev");
        execute_command("sudo mkdir -p " + target_folder + "/dev/pts");
        execute_command("sudo mkdir -p " + target_folder + "/proc");
        execute_command("sudo mkdir -p " + target_folder + "/sys");
        execute_command("sudo mkdir -p " + target_folder + "/run");
        execute_command("sudo mkdir -p " + target_folder + "/etc");

        execute_command("sudo mount --bind /dev " + target_folder + "/dev");
        execute_command("sudo mount --bind /dev/pts " + target_folder + "/dev/pts");
        execute_command("sudo mount --bind /proc " + target_folder + "/proc");
        execute_command("sudo mount --bind /sys " + target_folder + "/sys");
        execute_command("sudo mount --bind /run " + target_folder + "/run");
    }

    void unmount_system_dirs() {
        std::string target_folder = getTargetFolder();
        execute_command("sudo umount " + target_folder + "/dev/pts");
        execute_command("sudo umount " + target_folder + "/dev");
        execute_command("sudo umount " + target_folder + "/proc");
        execute_command("sudo umount " + target_folder + "/sys");
        execute_command("sudo umount " + target_folder + "/run");
    }

    void create_user() {
        std::string target_folder = getTargetFolder();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"useradd -m -G wheel -s /bin/bash " + new_username + "\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'root:" + root_password + "' | chpasswd\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo '" + new_username + ":" + user_password + "' | chpasswd\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\"");
    }

    void apply_timezone_keyboard_settings() {
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Setting timezone to: " << timezone << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"ln -sf /usr/share/zoneinfo/" + timezone + " /etc/localtime\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"hwclock --systohc\"");

        std::cout << COLOR_CYAN << "Setting keyboard layout to: " << keyboard_layout << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'KEYMAP=" + keyboard_layout + "' > /etc/vconsole.conf\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"locale-gen\"");
    }

    // ARCH DESKTOP INSTALLATION FUNCTIONS

    void install_arch_tty_grub() {
        current_desktop_name = "Arch-TTY-Grub";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch TTY Grub..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting Arch TTY Grub installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();
        std::cout << COLOR_CYAN << "Working from directory: " << currentDir << COLOR_RESET << std::endl;

        // CREATE TARGET DIRECTORY
        std::cout << COLOR_CYAN << "Creating target directory: " << target_folder << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder);

        // VERIFY DIRECTORY WAS CREATED
        struct stat info;
        if (stat(target_folder.c_str(), &info) != 0) {
            std::cerr << COLOR_RED << "Failed to create target directory: " << target_folder << COLOR_RESET << std::endl;
            return;
        }

        if (!(info.st_mode & S_IFDIR)) {
            std::cerr << COLOR_RED << "Target path is not a directory: " << target_folder << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_GREEN << "Target directory created successfully!" << COLOR_RESET << std::endl;

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL BASE SYSTEM WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing base system with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base " + selected_kernel + " linux-firmware grub efibootmgr os-prober sudo arch-install-scripts mkinitcpio vim nano bash-completion systemd networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        // VERIFY PACSTRAP SUCCESS
        std::string test_bin = target_folder + "/bin/bash";
        if (stat(test_bin.c_str(), &info) != 0) {
            std::cerr << COLOR_RED << "pacstrap failed! /bin/bash not found in target." << COLOR_RESET << std::endl;
            return;
        }

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        std::cout << COLOR_CYAN << "Installing GRUB..." << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Arch TTY Grub installation completed in: " << target_folder << COLOR_RESET << std::endl;

        // CREATE SQUASHFS IMAGE
        create_squashfs_image("Arch-TTY-Grub");
    }

    void install_gnome_desktop() {
        current_desktop_name = "Arch-GNOME";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch GNOME Desktop..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting GNOME installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL GNOME WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing GNOME Desktop with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base gnome gnome-extra gdm grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable gdm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "GNOME Desktop installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-GNOME");
    }

    void install_kde_plasma() {
        current_desktop_name = "Arch-KDE-Plasma";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch KDE Plasma..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting KDE Plasma installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL KDE WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing KDE Plasma with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base plasma sddm dolphin konsole kate grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "KDE Plasma installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-KDE-Plasma");
    }

    void install_xfce_desktop() {
        current_desktop_name = "Arch-XFCE";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch XFCE..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting XFCE installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL XFCE WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing XFCE with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base xfce4 xfce4-goodies lightdm lightdm-gtk-greeter grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable lightdm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "XFCE installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-XFCE");
    }

    void install_lxqt_desktop() {
        current_desktop_name = "Arch-LXQt";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch LXQt..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting LXQt installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL LXQT WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing LXQt with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base lxqt sddm grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "LXQt installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-LXQt");
    }

    void install_cinnamon_desktop() {
        current_desktop_name = "Arch-Cinnamon";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch Cinnamon..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting Cinnamon installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL CINNAMON WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing Cinnamon with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base cinnamon lightdm lightdm-gtk-greeter grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable lightdm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Cinnamon installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-Cinnamon");
    }

    void install_mate_desktop() {
        current_desktop_name = "Arch-MATE";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch MATE..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting MATE installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL MATE WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing MATE with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base mate mate-extra lightdm lightdm-gtk-greeter grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable lightdm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "MATE installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-MATE");
    }

    void install_budgie_desktop() {
        current_desktop_name = "Arch-Budgie";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch Budgie..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting Budgie installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL BUDGIE WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing Budgie with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base budgie-desktop lightdm lightdm-gtk-greeter grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable lightdm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Budgie installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-Budgie");
    }

    void install_i3_wm() {
        current_desktop_name = "Arch-i3";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch i3 (tiling WM)..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting i3 installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL i3 WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing i3 with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base i3-wm i3status i3lock dmenu lightdm lightdm-gtk-greeter grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable lightdm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "i3 installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-i3");
    }

    void install_sway_wm() {
        current_desktop_name = "Arch-Sway";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Installing Arch Sway (Wayland tiling)..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting Sway installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL SWAY WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing Sway with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base sway swaybg waybar wofi lightdm lightdm-gtk-greeter grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable lightdm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        // INSTALL GRUB
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB --recheck\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"mkinitcpio -P\"");

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Sway installation completed!" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-Sway");
    }

    void install_hyprland() {
        current_desktop_name = "Arch-Hyprland";
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_PURPLE << "Installing Arch Hyprland (Modern Wayland Compositor)..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting Hyprland installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        execute_command("sudo mkdir -p " + target_folder);

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");

        execute_command("sudo pacman -Sy");

        // INSTALL HYPRLAND WITH PACSTRAP
        std::cout << COLOR_CYAN << "Installing Hyprland with pacstrap..." << COLOR_RESET << std::endl;
        std::string packages = "base hyprland waybar rofi wl-clipboard sddm grub efibootmgr os-prober arch-install-scripts mkinitcpio " + selected_kernel + " linux-firmware sudo networkmanager";
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();
        create_user();

        

        unmount_system_dirs();
        std::cout << COLOR_PURPLE << "Hyprland installed! Note: You may need to configure ~/.config/hypr/hyprland.conf" << COLOR_RESET << std::endl;

        create_squashfs_image("Arch-Hyprland");
    }

    // Show desktop selection menu with ALL options
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
            selected = show_menu(desktop_options, "Select Desktop Environment", selected);

            switch(selected) {
                case 0:
                    install_arch_tty_grub();
                    break;
                case 1:
                    install_gnome_desktop();
                    break;
                case 2:
                    install_kde_plasma();
                    break;
                case 3:
                    install_xfce_desktop();
                    break;
                case 4:
                    install_lxqt_desktop();
                    break;
                case 5:
                    install_cinnamon_desktop();
                    break;
                case 6:
                    install_mate_desktop();
                    break;
                case 7:
                    install_budgie_desktop();
                    break;
                case 8:
                    install_i3_wm();
                    break;
                case 9:
                    install_sway_wm();
                    break;
                case 10:
                    install_hyprland();
                    break;
                case 11:
                    return;
            }

            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

    void show_main_menu() {
        std::vector<std::string> main_options = {
            "Installation Path: " + getTargetFolder(),
            "Set Username",
            "Set Root Password",
            "Set User Password",
            "Set Timezone",
            "Set Keyboard Layout",
            "Set Kernel",
            "Select Desktop Environment",
            "Exit"
        };

        int selected = 0;
        while (true) {
            system("clear");
            display_header();
            display_current_settings();

            // Update the first option to always show current path
            main_options[0] = "Installation Path: " + getTargetFolder();

            selected = show_menu(main_options, "Arch Linux Desktop Installer", selected);

            switch(selected) {
                case 0:
                    // Installation path is fixed, no action needed
                    break;
                case 1:
                    set_username();
                    break;
                case 2:
                    set_root_password();
                    break;
                case 3:
                    set_user_password();
                    break;
                case 4:
                    set_timezone();
                    break;
                case 5:
                    set_keyboard_layout();
                    break;
                case 6:
                    set_kernel();
                    break;
                case 7:
                    show_desktop_selection();
                    break;
                case 8:
                    std::cout << COLOR_GREEN << "Exiting. Goodbye!" << COLOR_RESET << std::endl;
                    return;
            }

            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

public:
    void run() {
        // EXTRACT FILES FIRST
        if (!extractRequiredFiles()) {
            std::cerr << COLOR_RED << "Failed to extract required files. Cannot continue." << COLOR_RESET << std::endl;
            return;
        }

        display_header();
        show_main_menu();
    }
};

int main() {
    ArchDesktopInstaller installer;
    installer.run();
    return 0;
}
