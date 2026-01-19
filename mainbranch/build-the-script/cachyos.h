#ifndef CACHYOS_H
#define CACHYOS_H

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

// Remove color definitions since they're in claudemods.h
// Include claudemods.h or declare extern references
extern const std::string COLOR_CYAN;
extern const std::string COLOR_RED;
extern const std::string COLOR_GREEN;
extern const std::string COLOR_YELLOW;
extern const std::string COLOR_ORANGE;
extern const std::string COLOR_PURPLE;
extern const std::string COLOR_RESET;

class CachyosInstaller {
private:
    std::string new_username;
    std::string root_password;
    std::string user_password;
    std::string timezone;
    std::string keyboard_layout;
    std::string current_distro_name; // Store the current distro name for ISO
    std::string extra_packages; // Store extra packages to install
    std::string selected_kernel; // Added missing variable
    std::string current_desktop_name; // Added missing variable

    // Terminal control for arrow keys
    struct termios oldt, newt;

    // Configuration file path
    std::string getConfigFilePath() {
        return getCurrentDir() + "/configurationcachyos.txt";
    }

    // Save configuration to file
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
            file.close();
            std::cout << COLOR_GREEN << "Configuration saved to " << configFile << COLOR_RESET << std::endl;
        } else {
            std::cerr << COLOR_RED << "Failed to save configuration to " << configFile << COLOR_RESET << std::endl;
        }
    }

    // Load configuration from file
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
                }
            }
            file.close();
            std::cout << COLOR_GREEN << "Configuration loaded from " << configFile << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_YELLOW << "No existing configuration found. Starting with default settings." << COLOR_RESET << std::endl;
        }
    }

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
        return getCurrentDir() + "/cachyos-distro";
    }

    // Get full target path (added missing method)
    std::string getFullTargetPath() {
        return getTargetFolder();
    }

    // Get calamares folder path
    std::string getCalamaresFolder() {
        return getCurrentDir() + "/calamares-cachyos";
    }

    // Check if directory exists
    bool directoryExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
    }

    // Check if file exists
    bool fileExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0);
    }

    // FIXED: Extract required files from calamares-per-distro - NO ERROR MESSAGES
    bool extractRequiredFiles() {
        std::cout << COLOR_CYAN << "Checking for required folders..." << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();
        std::string calamaresFolder = getCalamaresFolder();

        // Check if all required folders already exist in calamares-claudemods
        bool buildImageExists = directoryExists(calamaresFolder + "/build-image-arch-img");
        bool calamaresFilesExists = directoryExists(calamaresFolder + "/calamares-files");
        bool workingHooksExists = directoryExists(calamaresFolder + "/working-hooks-btrfs-ext4");

        if (buildImageExists && calamaresFilesExists && workingHooksExists) {
            std::cout << COLOR_GREEN << "All required folders already exist." << COLOR_RESET << std::endl;
            return true;
        }

        // Create calamares-claudemods folder if it doesn't exist
        if (!directoryExists(calamaresFolder)) {
            std::cout << COLOR_CYAN << "Creating calamares-cachyos folder..." << COLOR_RESET << std::endl;
            std::string createCmd = "sudo mkdir -p " + calamaresFolder;
            execute_command(createCmd);
        }

        // Extract the three ZIP files only if needed
        std::string sourceDir = currentDir + "/calamares-per-distro/cachyos";
        std::vector<std::string> zipFiles = {
            "calamares-cachyos.zip",
            "claudemods-cachyos.zip",
            "build-image-cachyos.zip"
        };

        for (const auto& zipFile : zipFiles) {
            std::string sourcePath = sourceDir + "/" + zipFile;

            // Check if source ZIP file exists
            if (fileExists(sourcePath)) {
                std::cout << COLOR_CYAN << "Extracting " << zipFile << "..." << COLOR_RESET << std::endl;

                // Extract silently without any error output
                std::string extractCmd = "sudo unzip -q " + sourcePath + " -d " + calamaresFolder + " >/dev/null 2>&1";
                execute_command(extractCmd);

                std::cout << COLOR_GREEN << "Done." << COLOR_RESET << std::endl;
            }
        }

        // Always return true - no error checking
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
        std::cout << COLOR_CYAN << "CachyOS distribution iso creator v1.01 19-01-2026" << COLOR_RESET << std::endl;
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

    // FIXED: Updated show_menu to show ALL values exactly as set (including passwords)
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

                // Create the menu line with proper formatting - SHOW ALL VALUES AS SET
                std::string menu_line;
                if (title == "CachyOS distribution iso creator") {
                    std::string setting_value;
                    switch(i) {
                        case 0: setting_value = getTargetFolder(); break;
                        case 1: setting_value = new_username.empty() ? "[Not Set]" : new_username; break;
                        case 2: setting_value = root_password.empty() ? "[Not Set]" : root_password; break; // Show actual password
                        case 3: setting_value = user_password.empty() ? "[Not Set]" : user_password; break; // Show actual password
                        case 4: setting_value = timezone.empty() ? "[Not Set]" : timezone; break;
                        case 5: setting_value = keyboard_layout.empty() ? "[Not Set]" : keyboard_layout; break;
                        case 6: setting_value = selected_kernel.empty() ? "[Not Set]" : selected_kernel; break;
                        case 7: setting_value = current_desktop_name.empty() ? "[Not Set]" : current_desktop_name; break;
                        case 8: setting_value = extra_packages.empty() ? "[Not Set]" : extra_packages; break;
                        default: setting_value = ""; break;
                    }

                    // For installation path (i=0), don't add setting_value to avoid duplication
                    if (i == 0) {
                        menu_line = options[i];
                    } else {
                        menu_line = options[i] + " " + setting_value;
                    }
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

    // FIXED: Function to display current settings on main menu - now shows ALL values as set
    void display_current_settings() {
        std::cout << COLOR_YELLOW << "\nCurrent Settings:" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Installation Path: " << COLOR_RESET << getTargetFolder() << std::endl;
        std::cout << COLOR_CYAN << "Username: " << COLOR_RESET
        << (new_username.empty() ? "[Not Set]" : new_username) << std::endl;
        std::cout << COLOR_CYAN << "Root Password: " << COLOR_RESET
        << (root_password.empty() ? "[Not Set]" : root_password) << std::endl; // Show actual password
        std::cout << COLOR_CYAN << "User Password: " << COLOR_RESET
        << (user_password.empty() ? "[Not Set]" : user_password) << std::endl; // Show actual password
        std::cout << COLOR_CYAN << "Timezone: " << COLOR_RESET
        << (timezone.empty() ? "[Not Set]" : timezone) << std::endl;
        std::cout << COLOR_CYAN << "Keyboard Layout: " << COLOR_RESET
        << (keyboard_layout.empty() ? "[Not Set]" : keyboard_layout) << std::endl;
        std::cout << COLOR_CYAN << "Kernel: " << COLOR_RESET
        << (selected_kernel.empty() ? "[Not Set]" : selected_kernel) << std::endl;
        std::cout << COLOR_CYAN << "Desktop Environment: " << COLOR_RESET
        << (current_desktop_name.empty() ? "[Not Set]" : current_desktop_name) << std::endl;
        std::cout << COLOR_CYAN << "Extra Packages: " << COLOR_RESET
        << (extra_packages.empty() ? "[Not Set]" : extra_packages) << std::endl;
        std::cout << std::endl;
    }

    // NEW: Common installation setup (shared by all desktop environments)
    bool setup_installation_environment() {
        std::string target_folder = getFullTargetPath();
        std::string currentDir = getCurrentDir();

        // CREATE TARGET DIRECTORY
        std::cout << COLOR_CYAN << "Creating target directory: " << target_folder << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder);

        // VERIFY DIRECTORY WAS CREATED
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

        // CREATE ESSENTIAL DIRECTORIES
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");
        execute_command("sudo mkdir -p " + target_folder + "/usr/");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib/initcpio/");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib/initcpio/udev/");

        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/needed-files/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");
        execute_command("sudo unzip -o " + currentDir + "/needed-files/pacman.d.zip -d " + target_folder + "/etc/pacman.d");
        execute_command("sudo unzip -o " + currentDir + "/needed-files/pacman.d.zip -d /etc/pacman.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/pacman.conf " + target_folder + "/etc/pacman.conf");
        execute_command("sudo cp -r " + currentDir + "/needed-files/pacman.conf /etc/pacman.conf");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules " + target_folder + "/usr/lib/initcpio/udev/11-dm-initramfs.rules");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules /usr/lib/initcpio/udev/11-dm-initramfs.rules");

        execute_command("sudo pacman -Sy");
        return true;
    }

    // NEW: Common package installation function
    bool install_base_packages(const std::string& desktop_packages, const std::string& display_manager = "") {
        std::string target_folder = getFullTargetPath();
        std::string currentDir = getCurrentDir();  // ADD THIS LINE

        // BUILD COMPLETE PACKAGE LIST - USING CACHYOS PACKAGES
        std::string packages;

        // Set package based on desktop selection
        if (current_desktop_name == "CachyOS-TTY-Grub") {
            packages = "cachyosttygrub calamares-fix";
        } else if (current_desktop_name == "CachyOS-KDE-Grub") {
            packages = "cachyoskdegrub calamares-fix";
        } else if (current_desktop_name == "CachyOS-GNOME-Grub") {
            packages = "cachyosgnomegrub calamares-fix";
        }

        // Add extra packages if specified
        if (!extra_packages.empty()) {
            packages += " " + extra_packages;
        }

        std::cout << COLOR_CYAN << "Installing packages with pacstrap..." << COLOR_RESET << std::endl;
        execute_command("sudo pacstrap " + target_folder + " " + packages);

        // VERIFY PACSTRAP SUCCESS
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

    // NEW: Common service enablement
    void enable_services(const std::string& display_manager_service) {
        std::string target_folder = getFullTargetPath();

        mount_system_dirs();

        // Always enable NetworkManager
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        // Enable display manager if specified
        if (!display_manager_service.empty()) {
            execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable " + display_manager_service + "\"");
        }

        apply_timezone_keyboard_settings();
        create_user();
    }

    // NEW: Common post-installation steps
    void complete_installation(const std::string& desktop_name) {
        std::string target_folder = getFullTargetPath();

        // Install Calamares
        install_calamares();

        unmount_system_dirs();
        std::cout << COLOR_GREEN << desktop_name << " installation completed!" << COLOR_RESET << std::endl;

        // CREATE SQUASHFS IMAGE
        create_squashfs_image(desktop_name);
    }

    // NEW: Dedicated function to install Calamares (eliminates code duplication)
    void install_calamares() {
        std::string target_folder = getFullTargetPath();
        std::string currentDir = getCurrentDir();

        std::cout << COLOR_CYAN << "Installing Calamares installer and setting up iso ..." << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Desktop");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Documents");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Downloads");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Music");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Pictures");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Videos");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Public");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Templates");
        // Copy Calamares package files

        // Copy calamares config
        execute_command("sudo cp -r " + currentDir + "/calamares-cachyos/calamares-files/calamares " + target_folder + "/etc/");

        // Copy custom branding
        execute_command("sudo cp -r " + currentDir + "/calamares-cachyos/calamares-files/claudemods " + target_folder + "/usr/share/calamares/branding/");

        // Extract extra files
        execute_command("sudo unzip -o -q " + currentDir + "/calamares-cachyos/calamares-files/extras.zip -d " + target_folder);

        // Copy hooks
        execute_command("sudo cp -r " + currentDir + "/calamares-cachyos/working-hooks-btrfs-ext4/* /etc/initcpio");

        execute_command("sudo cp " + currentDir + "/calamares-cachyos/calamares-files/mount.conf " + target_folder + "/usr/share/calamares/modules");

        // Copy desktop shortcuts
        execute_command("sudo cp " + currentDir + "/needed-files/Calamares " + target_folder + "/home/" + new_username + "/Desktop");
        execute_command("sudo cp " + currentDir + "/needed-files/rsync-installer " + target_folder + "/home/" + new_username + "/Desktop");
        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/Desktop/Calamares");
        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/Desktop/rsync-installer");
        execute_command("sudo mkdir -p " + target_folder + "/opt/rsync-installer");
        execute_command("sudo tar xzf " + currentDir + "/needed-files/rsync-installer.tar.gz -C " + target_folder + "/opt/rsync-installer");

        // Remove manjaro branding
        execute_command("sudo rm -rf " + target_folder + "/usr/share/calamares/branding/manjaro");


        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.config/fish");
        execute_command("sudo cp " + currentDir + "/needed-files/fish_variables " + target_folder + "/home/" + new_username + "/.config/fish/fish_variables");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"su " + new_username + " -c 'chsh -s $(which fish)'\"");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.local/share");

        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub " + target_folder + "/etc/default/grub");
        execute_command("sudo unzip -o -q " + currentDir + "/needed-files/bootcachyos.zip -d " + target_folder + "/boot");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/cachyos-iso-initramfs/kernel-latest.img");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/cachyos-iso-initramfs/initramfs-x86_64.img");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/cachyos-iso-initramfs/vmlinuz-x86_64.zip");
        execute_command("sudo unsquashfs -d " + target_folder + "/usr/lib/modules/6.18.6-2-cachyos " + currentDir + "/kernel-latest.img");
        execute_command("sudo cp initramfs-x86_64.img " + currentDir + "/calamares-cachyos/build-image-arch-img/boot/initramfs-x86_64.img");
        execute_command("sudo unzip -o " + currentDir + "/vmlinuz-x86_64.zip -d " + currentDir + "/calamares-cachyos/build-image-arch-img/boot");
        execute_command("sudo rm -rf kernel-latest.img");
        execute_command("sudo rm -rf initramfs-x86_64.img");
        execute_command("sudo rm -rf *.zip");

        std::cout << COLOR_GREEN << "Calamares installation and iso setup completed!" << COLOR_RESET << std::endl;
    }

    // UPDATED: Function to create squashfs image after installation with additional steps
    void create_squashfs_image(const std::string& distro_name) {
        std::cout << COLOR_CYAN << "Creating squashfs image..." << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();
        std::string target_folder = getFullTargetPath();

        // NEW: Clean pacman cache before creating squashfs
        std::cout << COLOR_CYAN << "Cleaning pacman cache..." << COLOR_RESET << std::endl;
        std::string cache_clean_cmd = "sudo rm -rf " + target_folder + "/var/cache/pacman/pkg/*";
        if (execute_command(cache_clean_cmd) == 0) {
            std::cout << COLOR_GREEN << "Pacman cache cleaned successfully!" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "Failed to clean pacman cache!" << COLOR_RESET << std::endl;
        }

        std::string squashfs_cmd = "sudo mksquashfs " + target_folder + " " + currentDir + "/calamares-cachyos/build-image-arch-img/LiveOS/rootfs.img -noappend -comp xz -b 256K -Xbcj x86 -e etc/udev/rules.d/70-persistent-cd.rules -e etc/udev/rules.d/70-persistent-net.rules -e etc/mtab -e etc/fstab -e dev/* -e proc/* -e sys/* -e tmp/* -e run/* -e mnt/* -e media/* -e lost+found";

        std::cout << COLOR_CYAN << "Executing: " << squashfs_cmd << COLOR_RESET << std::endl;

        if (execute_command(squashfs_cmd) == 0) {
            std::cout << COLOR_GREEN << "Squashfs image created successfully!" << COLOR_RESET << std::endl;

            // Clean up target folder after successful squashfs creation
            std::cout << COLOR_CYAN << "Cleaning up target folder..." << COLOR_RESET << std::endl;
            std::string cleanup_cmd = "sudo rm -rf " + target_folder;
            if (execute_command(cleanup_cmd) == 0) {
                std::cout << COLOR_GREEN << "Target folder deleted successfully: " << target_folder << COLOR_RESET << std::endl;
            }

            create_iso_image(distro_name);
        } else {
            std::cout << COLOR_RED << "Failed to create squashfs image!" << COLOR_RESET << std::endl;
        }
    }

    void create_iso_image(const std::string& distro_name) {
        std::cout << COLOR_CYAN << "Creating ISO image with XORRISO..." << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();
        std::string currentDIR = currentDir;

        // Build the XORRISO command with the distro name as ISO filename
        std::string xorriso_cmd = "sudo xorriso -as mkisofs "
        "--modification-date=\"$(date +%Y%m%d%H%M%S00)\" "
        "--protective-msdos-label "
        "-volid \"2025\" "
        "-appid \"CachyOS Linux Live/Rescue CD\" "
        "-publisher \"CachyOS >\" "
        "-preparer \"Prepared by user\" "
        "-r -graft-points -no-pad "
        "--sort-weight 0 / "
        "--sort-weight 1 /boot "
        "--grub2-mbr " + currentDir + "/calamares-cachyos/build-image-arch-img/boot/grub/i386-pc/boot_hybrid.img "
        "-partition_offset 16 "
        "-b boot/grub/i386-pc/eltorito.img "
        "-c boot.catalog "
        "-no-emul-boot -boot-load-size 4 -boot-info-table --grub2-boot-info "
        "-eltorito-alt-boot "
        "-append_partition 2 0xef " + currentDir + "/calamares-cachyos/build-image-arch-img/boot/efi.img "
        "-e --interval:appended_partition_2:all:: "
        "-no-emul-boot "
        "-iso-level 3 "
        "-o \"" + currentDir + "/" + distro_name + ".iso\" " +
        currentDir + "/calamares-cachyos/build-image-arch-img/";

        std::cout << COLOR_CYAN << "Executing: " << xorriso_cmd << COLOR_RESET << std::endl;

        if (execute_command(xorriso_cmd) == 0) {
            std::cout << COLOR_GREEN << "ISO image created successfully: " << distro_name + ".iso" << COLOR_RESET << std::endl;

            // Get username using whoami
            FILE* whoami_pipe = popen("whoami", "r");
            if (whoami_pipe) {
                char username[256];
                if (fgets(username, sizeof(username), whoami_pipe)) {
                    // Remove newline
                    username[strcspn(username, "\n")] = 0;
                    // Change ownership to current user
                    std::string chown_cmd = "sudo chown " + std::string(username) + ":" + std::string(username) + " \"" + currentDir + "/" + distro_name + ".iso\"";
                    execute_command(chown_cmd);
                }
                pclose(whoami_pipe);
            }
        } else {
            std::cout << COLOR_RED << "Failed to create ISO image!" << COLOR_RESET << std::endl;
        }
    }

    // Set username
    void set_username() {
        new_username = get_input("Enter username: ");
        saveConfiguration();
    }

    // Set root password
    void set_root_password() {
        root_password = get_input("Enter root password: ");
        saveConfiguration();
    }

    // Set user password
    void set_user_password() {
        user_password = get_input("Enter user password: ");
        saveConfiguration();
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
        saveConfiguration();
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
        saveConfiguration();
    }

    // Set kernel - CHANGED TO CACHYOS KERNELS
    void set_kernel() {
        std::vector<std::string> kernel_options = {
            "linux-cachyos (Default, Recommended)",
            "linux-cachyos-bore (General desktop performance)",
            "linux-cachyos-eevdf (Improved Responsiveness)",
            "linux-cachyos-lts (Stability)",
            "linux-cachyos-rt-bore (Real-Time Preemption)",
            "linux-cachyos-hardened (Security)",
            "linux-cachyos-server (Server Workloads)",
            "linux-cachyos-deckify (Handheld Devices)",
            "linux-cachyos-rc (Development / Latest Mainline)",
            "linux-cachyos-bmq (BMQ Scheduler)"
        };

        int kernel_choice = show_menu(kernel_options, "Select CachyOS Kernel");
        switch(kernel_choice) {
            case 0: selected_kernel = "linux-cachyos"; break;
            case 1: selected_kernel = "linux-cachyos-bore"; break;
            case 2: selected_kernel = "linux-cachyos-eevdf"; break;
            case 3: selected_kernel = "linux-cachyos-lts"; break;
            case 4: selected_kernel = "linux-cachyos-rt-bore"; break;
            case 5: selected_kernel = "linux-cachyos-hardened"; break;
            case 6: selected_kernel = "linux-cachyos-server"; break;
            case 7: selected_kernel = "linux-cachyos-deckify"; break;
            case 8: selected_kernel = "linux-cachyos-rc"; break;
            case 9: selected_kernel = "linux-cachyos-bmq"; break;
        }
        saveConfiguration();
    }

    // NEW: Set extra packages
    void set_extra_packages() {
        std::cout << COLOR_CYAN << "Enter additional packages to install (space-separated):" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Examples: vim git htop curl wget firefox" << COLOR_RESET << std::endl;
        extra_packages = get_input("Extra packages: ");
        saveConfiguration();
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
        if (current_desktop_name.empty()) {
            std::cout << COLOR_RED << "Error: Desktop environment not selected!" << COLOR_RESET << std::endl;
            return false;
        }
        return true;
    }

    void mount_system_dirs() {
        std::string target_folder = getFullTargetPath();
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
        std::string target_folder = getFullTargetPath();
        execute_command("sudo umount " + target_folder + "/dev/pts");
        execute_command("sudo umount " + target_folder + "/dev");
        execute_command("sudo umount " + target_folder + "/proc");
        execute_command("sudo umount " + target_folder + "/sys");
        execute_command("sudo umount " + target_folder + "/run");
    }

    void create_user() {
        std::string target_folder = getFullTargetPath();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"useradd -m -G wheel -s /bin/bash " + new_username + "\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'root:" + root_password + "' | chpasswd\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo '" + new_username + ":" + user_password + "' | chpasswd\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\"");
    }

    void apply_timezone_keyboard_settings() {
        std::string target_folder = getFullTargetPath();
        std::cout << COLOR_CYAN << "Setting timezone to: " << timezone << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"ln -sf /usr/share/zoneinfo/" + timezone + " /etc/localtime\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"hwclock --systohc\"");

        std::cout << COLOR_CYAN << "Setting keyboard layout to: " << keyboard_layout << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'KEYMAP=" + keyboard_layout + "' > /etc/vconsole.conf\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"locale-gen\"");
    }

    // NEW: Start installation function
    void start_installation() {
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        std::cout << COLOR_CYAN << "Starting installation with selected desktop: " << current_desktop_name << COLOR_RESET << std::endl;

        // Call the appropriate installation function based on current_desktop_name
        if (current_desktop_name == "CachyOS-TTY-Grub") {
            install_cachyos_tty_grub();
        } else if (current_desktop_name == "CachyOS-KDE-Grub") {
            install_cachyos_kde_grub();
        } else if (current_desktop_name == "CachyOS-GNOME-Grub") {
            install_cachyos_gnome_grub();
        } else {
            std::cout << COLOR_RED << "Unknown desktop environment: " << current_desktop_name << COLOR_RESET << std::endl;
        }
    }

    // UPDATED: Simplified desktop installation functions using common helpers

    void install_cachyos_tty_grub() {
        std::cout << COLOR_CYAN << "Installing CachyOS TTY Grub..." << COLOR_RESET << std::endl;

        if (!setup_installation_environment()) return;

        if (!install_base_packages("", "")) return;

        // Enable services (no display manager for TTY)
        enable_services("");

        complete_installation("CachyOS-TTY-Grub");
    }

    void install_cachyos_kde_grub() {
        std::cout << COLOR_CYAN << "Installing CachyOS KDE Grub..." << COLOR_RESET << std::endl;

        if (!setup_installation_environment()) return;

        if (!install_base_packages("", "")) return;

        enable_services("sddm");
        complete_installation("CachyOS-KDE-Grub");
    }

    void install_cachyos_gnome_grub() {
        std::cout << COLOR_CYAN << "Installing CachyOS GNOME Grub..." << COLOR_RESET << std::endl;

        if (!setup_installation_environment()) return;

        if (!install_base_packages("", "")) return;

        enable_services("gdm");
        complete_installation("CachyOS-GNOME-Grub");
    }

    // Show desktop selection menu with ONLY 3 CACHYOS OPTIONS
    void show_desktop_selection() {
        std::vector<std::string> desktop_options = {
            "CachyOS TTY Grub (Terminal Only)",
            "CachyOS KDE Grub",
            "CachyOS GNOME Grub",
            "Back to Main Menu"
        };

        int selected = 0;
        while (true) {
            selected = show_menu(desktop_options, "Select CachyOS Desktop Environment", selected);

            switch(selected) {
                case 0:
                    current_desktop_name = "CachyOS-TTY-Grub";
                    std::cout << COLOR_GREEN << "Desktop environment set to: CachyOS TTY Grub" << COLOR_RESET << std::endl;
                    saveConfiguration();
                    return;
                case 1:
                    current_desktop_name = "CachyOS-KDE-Grub";
                    std::cout << COLOR_GREEN << "Desktop environment set to: CachyOS KDE Grub" << COLOR_RESET << std::endl;
                    saveConfiguration();
                    return;
                case 2:
                    current_desktop_name = "CachyOS-GNOME-Grub";
                    std::cout << COLOR_GREEN << "Desktop environment set to: CachyOS GNOME Grub" << COLOR_RESET << std::endl;
                    saveConfiguration();
                    return;
                case 3:
                    return;
            }
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
            "Install Extra Packages",
            "Start Installation",
            "Exit"
        };

        int selected = 0;
        while ( true) {
            system("clear");
            display_header();
            display_current_settings();

            // Update the first option to always show current path
            main_options[0] = "Installation Path: " + getTargetFolder();

            selected = show_menu(main_options, "CachyOS distribution iso creator", selected);

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
                    set_extra_packages();
                    break;
                case 9:
                    start_installation();
                    break;
                case 10:
                    std::cout << COLOR_GREEN << "Exiting. Goodbye!" << COLOR_RESET << std::endl;
                    return;
            }

            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

public:
    void run() {
        // LOAD CONFIGURATION FIRST - FIXED: This now happens before anything else
        loadConfiguration();

        // EXTRACT FILES
        if (!extractRequiredFiles()) {
            std::cerr << COLOR_RED << "Failed to extract required files. Cannot continue." << COLOR_RESET << std::endl;
            return;
        }

        display_header();
        show_main_menu();
    }
};

#endif // CACHYOS_H
