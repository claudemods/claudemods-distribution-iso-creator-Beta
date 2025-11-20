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

class ClaudemodsInstaller {
private:
    std::string new_username;
    std::string root_password;
    std::string user_password;
    std::string timezone;
    std::string keyboard_layout;
    std::string current_distro_name;
    std::string selected_kernel;
    std::string current_desktop_name;
    std::string extra_packages;
    
    struct termios oldt, newt;
    std::string config_file = "configurationarch.txt";
    
    // FIXED: WORKING CONFIGURATION SYSTEM
    void save_configuration() {
        FILE* config = fopen(config_file.c_str(), "w");
        if (config) {
            fprintf(config, "username=%s\n", new_username.c_str());
            fprintf(config, "root_password=%s\n", root_password.c_str());
            fprintf(config, "user_password=%s\n", user_password.c_str());
            fprintf(config, "timezone=%s\n", timezone.c_str());
            fprintf(config, "keyboard_layout=%s\n", keyboard_layout.c_str());
            fprintf(config, "kernel=%s\n", selected_kernel.c_str());
            fprintf(config, "desktop=%s\n", current_desktop_name.c_str());
            fprintf(config, "extra_packages=%s\n", extra_packages.c_str());
            fclose(config);
        }
    }
    
    // FIXED: Improved load_configuration function
    void load_configuration() {
        FILE* config = fopen(config_file.c_str(), "r");
        if (!config) return;
        
        char line[256];
        while (fgets(line, sizeof(line), config)) {
            char* equals = strchr(line, '=');
            if (equals) {
                *equals = '\0';
                char* value = equals + 1;
                // Remove newline from value
                char* newline = strchr(value, '\n');
                if (newline) *newline = '\0';
                
                std::string key = line;
                std::string val = value;
                
                if (key == "username") new_username = val;
                else if (key == "root_password") root_password = val;
                else if (key == "user_password") user_password = val;
                else if (key == "timezone") timezone = val;
                else if (key == "keyboard_layout") keyboard_layout = val;
                else if (key == "kernel") selected_kernel = val;
                else if (key == "desktop") current_desktop_name = val;
                else if (key == "extra_packages") extra_packages = val;
            }
        }
        fclose(config);
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
        return "current directory as claudemods-distro";
    }
    
    // Get full target path for actual operations
    std::string getFullTargetPath() {
        return getCurrentDir() + "/claudemods-distro";
    }
    
    // Check if directory exists
    bool directoryExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
    }
    
    // MODIFIED: Extract required files only if folders don't exist
    bool extractRequiredFiles() {
        std::cout << COLOR_CYAN << "Checking for required folders..." << COLOR_RESET << std::endl;
        
        std::string currentDir = getCurrentDir();
        bool buildImageExists = directoryExists(currentDir + "/build-image-arch-img");
        bool calamaresFilesExists = directoryExists(currentDir + "/calamares-files");
        bool workingHooksExists = directoryExists(currentDir + "/working-hooks-btrfs-ext4");
        
        // Check if all required folders already exist
        if (buildImageExists && calamaresFilesExists && workingHooksExists) {
            std::cout << COLOR_GREEN << "All required folders already exist. Skipping extraction." << COLOR_RESET << std::endl;
            return true;
        }
        
        std::cout << COLOR_CYAN << "Some folders missing, extracting required files..." << COLOR_RESET << std::endl;
        
        // Extract only what's needed
        if (!buildImageExists) {
            std::cout << COLOR_CYAN << "Extracting build-image-arch-img..." << COLOR_RESET << std::endl;
        }
        if (!calamaresFilesExists) {
            std::cout << COLOR_CYAN << "Extracting calamares files..." << COLOR_RESET << std::endl;
        }
        if (!workingHooksExists) {
            std::cout << COLOR_CYAN << "Extracting working hooks..." << COLOR_RESET << std::endl;
        }
        
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
        std::cout << "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░██████╗" << std::endl;
        std::cout << "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝" << std::endl;
        std::cout << "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░" << std::endl;
        std::cout << "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗" << std::endl;
        std::cout << "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝" << std::endl;
        std::cout << "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░" << std::endl;
        std::cout << COLOR_CYAN << "claudemods Arch Linux distribution iso creator v1.0 20-11-2025" << COLOR_RESET << std::endl;
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
                if (title == "claudemods Arch Linux distribution iso creator") {
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
        
        // COPY CONFIGURATION FILES
        execute_command("sudo cp -r " + currentDir + "/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");
        execute_command("sudo unzip -o " + currentDir + "/archpacman.d.zip -d " + target_folder + "/etc/pacman.d");
        execute_command("sudo unzip -o " + currentDir + "/archpacman.d.zip -d /etc/pacman.d");
        execute_command("sudo cp -r " + currentDir + "/archpacman.conf " + target_folder + "/etc/pacman.conf");
        execute_command("sudo cp -r " + currentDir + "/archpacman.conf /etc/pacman.conf");
        
        execute_command("sudo pacman -Sy");
        return true;
    }
    
    // NEW: Common package installation function for Arch Linux
    bool install_base_packages(const std::string& desktop_packages, const std::string& display_manager = "") {
        std::string target_folder = getFullTargetPath();
        
        // BUILD COMPLETE PACKAGE LIST - USING ARCH LINUX PACKAGES
        std::string packages = "base base-devel linux-firmware grub efibootmgr os-prober sudo arch-install-scripts mkinitcpio vim nano bash-completion systemd networkmanager " + selected_kernel;
        
        // Add desktop environment packages
        if (!desktop_packages.empty()) {
            packages += " " + desktop_packages;
        }
        
        // Add display manager if specified
        if (!display_manager.empty()) {
            packages += " " + display_manager;
        }
        
        // Add extra packages if specified
        if (!extra_packages.empty()) {
            packages += " " + extra_packages;
        }
        
        std::cout << COLOR_CYAN << "Installing packages with pacstrap..." << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Package list: " << packages << COLOR_RESET << std::endl;
        
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
        
        std::cout << COLOR_CYAN << "Installing Calamares installer..." << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Desktop");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Documents");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Downloads");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Music");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Pictures");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Videos");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Public");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/Templates");
        // Copy Calamares package files
        execute_command("sudo cp " + currentDir + "/calamares-files/calamares-3.4.0-1-x86_64.pkg.tar.zst " + target_folder);
        execute_command("sudo cp " + currentDir + "/calamares-files/calamares-oem-kde-settings-20240616-3-any.pkg.tar " + target_folder);
        execute_command("sudo cp " + currentDir + "/calamares-files/calamares-tools-0.1.0-1-any.pkg.tar.zst " + target_folder);
        execute_command("sudo cp " + currentDir + "/calamares-files/ckbcomp-1.227-2-any.pkg.tar " + target_folder);
        
        // Install in chroot
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"pacman -U *.pkg.tar* --noconfirm\"");
        
        // Copy calamares config
        execute_command("sudo cp -r " + currentDir + "/calamares-files/calamares " + target_folder + "/etc/");
        
        // Copy custom branding
        execute_command("sudo cp -r " + currentDir + "/calamares-files/claudemods " + target_folder + "/usr/share/calamares/branding/");
        
        // Extract extra files
        execute_command("sudo unzip -o -q " + currentDir + "/calamares-files/extras.zip -d " + target_folder);
        
        // Copy hooks
        execute_command("sudo cp -r " + currentDir + "/working-hooks-btrfs-ext4/* /etc/initcpio");
        
        execute_command("sudo cp " + currentDir + "/calamares-files/mount.conf " + target_folder + "/usr/share/calamares/modules");
        
        // Copy desktop shortcuts
        execute_command("sudo cp " + currentDir + "/Calamares " + target_folder + "/home/" + new_username + "/Desktop");
        execute_command("sudo cp " + currentDir + "/rsync-installer " + target_folder + "/home/" + new_username + "/Desktop");
        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/Desktop/Calamares");
        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/Desktop/rsync-installer");
        execute_command("sudo mkdir -p " + target_folder + "/opt/rsync-installer");
        execute_command("sudo tar xzf " + currentDir + "/rsync-installer.tar.gz -C " + target_folder + "/opt/rsync-installer");
        
        // Remove manjaro branding
        execute_command("sudo rm -rf " + target_folder + "/usr/share/calamares/branding/manjaro");
        
        // Delete the package files from target folder
        execute_command("sudo rm -f " + target_folder + "/calamares-3.4.0-1-x86_64.pkg.tar.zst");
        execute_command("sudo rm -f " + target_folder + "/calamares-oem-kde-settings-20240616-3-any.pkg.tar");
        execute_command("sudo rm -f " + target_folder + "/calamares-tools-0.1.0-1-any.pkg.tar.zst");
        execute_command("sudo rm -f " + target_folder + "/ckbcomp-1.227-2-any.pkg.tar");
        
    
        std::cout << COLOR_GREEN << "Calamares installation completed!" << COLOR_RESET << std::endl;
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
        
        std::string squashfs_cmd = "sudo mksquashfs " + target_folder + " " + currentDir + "/build-image-arch-img/LiveOS/rootfs.img -noappend -comp xz -b 256K -Xbcj x86 -e etc/udev/rules.d/70-persistent-cd.rules -e etc/udev/rules.d/70-persistent-net.rules -e etc/mtab -e etc/fstab -e dev/* -e proc/* -e sys/* -e tmp/* -e run/* -e mnt/* -e media/* -e lost+found";
        
        std::cout << COLOR_CYAN << "Executing: " << squashfs_cmd << COLOR_RESET << std::endl;
        
        if (execute_command(squashfs_cmd) == 0) {
            std::cout << COLOR_GREEN << "Squashfs image created successfully!" << COLOR_RESET << std::endl;
            
            // Clean up target folder after successful squashfs creation
            std::cout << COLOR_CYAN << "Cleaning up target folder..." << COLOR_RESET << std::endl;
            std::string cleanup_cmd = "sudo rm -rf " + target_folder;
            if (execute_command(cleanup_cmd) == 0) {
                std::cout << COLOR_GREEN << "Target folder deleted successfully: " << target_folder << COLOR_RESET << std::endl;
            }
            
            // Copy kernel image with user selection
            std::cout << COLOR_CYAN << "Searching for kernel images..." << COLOR_RESET << std::endl;
            
            // Get list of vmlinuz files
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
            
            if (kernel_files.empty()) {
                std::cout << COLOR_RED << "No kernel images found!" << COLOR_RESET << std::endl;
            } else {
                // Show available kernels
                std::cout << COLOR_CYAN << "Available kernels:" << COLOR_RESET << std::endl;
                for (size_t i = 0; i < kernel_files.size(); i++) {
                    std::cout << "[" << i + 1 << "] " << kernel_files[i] << std::endl;
                }
                
                // Get selection
                std::cout << COLOR_CYAN << "Select kernel (1-" << kernel_files.size() << "): " << COLOR_RESET;
                std::string input;
                std::getline(std::cin, input);
                
                try {
                    int choice = std::stoi(input);
                    if (choice >= 1 && choice <= kernel_files.size()) {
                        std::string copy_cmd = "sudo cp " + kernel_files[choice - 1] + " " + currentDir + "/build-image-arch-img/boot/vmlinuz-x86_64";
                        if (execute_command(copy_cmd) == 0) {
                            std::cout << COLOR_GREEN << "Kernel copied successfully!" << COLOR_RESET << std::endl;
                        } else {
                            std::cout << COLOR_RED << "Failed to copy kernel!" << COLOR_RESET << std::endl;
                        }
                    }
                } catch (...) {
                    std::cout << COLOR_RED << "Invalid selection!" << COLOR_RESET << std::endl;
                }
            }
            
            // Generate initramfs
            std::cout << COLOR_CYAN << "Generating initramfs..." << COLOR_RESET << std::endl;
            std::string initramfs_cmd = "cd " + currentDir + "/build-image-arch-img && sudo mkinitcpio -c mkinitcpio.conf -g " + currentDir + "/build-image-arch-img/boot/initramfs-x86_64.img";
            if (execute_command(initramfs_cmd) == 0) {
                std::cout << COLOR_GREEN << "Initramfs generated successfully!" << COLOR_RESET << std::endl;
                create_iso_image(distro_name);
            }
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
        "-appid \"claudemods Linux Live/Rescue CD\" "
        "-publisher \"claudemods claudemods101@gmail.com >\" "
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
        "-o \"" + currentDir + "/" + distro_name + ".iso\" " +
        currentDir + "/build-image-arch-img/";
        
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
        save_configuration();
    }
    
    // Set root password
    void set_root_password() {
        root_password = get_input("Enter root password: ");
        save_configuration();
    }
    
    // Set user password
    void set_user_password() {
        user_password = get_input("Enter user password: ");
        save_configuration();
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
        save_configuration();
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
        save_configuration();
    }
    
    // Set kernel - CHANGED TO ARCH LINUX KERNELS
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
        save_configuration();
    }
    
    // NEW: Set extra packages
    void set_extra_packages() {
        std::cout << COLOR_CYAN << "Enter additional packages to install (space-separated):" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Examples: vim git htop curl wget firefox" << COLOR_RESET << std::endl;
        extra_packages = get_input("Extra packages: ");
        save_configuration();
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
        if (current_desktop_name == "Arch-TTY-Grub") {
            install_arch_tty_grub();
        } else if (current_desktop_name == "GNOME-Desktop") {
            install_gnome_desktop();
        } else if (current_desktop_name == "KDE-Plasma") {
            install_kde_plasma();
        } else if (current_desktop_name == "XFCE-Desktop") {
            install_xfce_desktop();
        } else if (current_desktop_name == "LXQt-Desktop") {
            install_lxqt_desktop();
        } else if (current_desktop_name == "Cinnamon-Desktop") {
            install_cinnamon_desktop();
        } else if (current_desktop_name == "MATE-Desktop") {
            install_mate_desktop();
        } else if (current_desktop_name == "Budgie-Desktop") {
            install_budgie_desktop();
        } else if (current_desktop_name == "i3-WM") {
            install_i3_wm();
        } else if (current_desktop_name == "Sway-WM") {
            install_sway_wm();
        } else if (current_desktop_name == "Hyprland-WM") {
            install_hyprland_wm();
        } else {
            std::cout << COLOR_RED << "Unknown desktop environment: " << current_desktop_name << COLOR_RESET << std::endl;
        }
    }
    
    // UPDATED: Desktop installation functions for Arch Linux
    
    void install_arch_tty_grub() {
        std::cout << COLOR_CYAN << "Installing Arch Linux TTY Grub..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("", "")) return;
        
        // Enable services (no display manager for TTY)
        enable_services("");
        
        complete_installation("Arch-TTY-Grub");
    }
    
    void install_gnome_desktop() {
        std::cout << COLOR_CYAN << "Installing GNOME Desktop..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("gnome gnome-extra", "gdm")) return;
        
        enable_services("gdm");
        complete_installation("Arch-GNOME-Desktop");
    }
    
    void install_kde_plasma() {
        std::cout << COLOR_CYAN << "Installing KDE Plasma..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("plasma dolphin konsole kate", "sddm")) return;
        
        enable_services("sddm");
        complete_installation("Arch-KDE-Plasma");
    }
    
    void install_xfce_desktop() {
        std::cout << COLOR_CYAN << "Installing XFCE Desktop..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("xfce4 xfce4-goodies lightdm lightdm-gtk-greeter", "lightdm")) return;
        
        enable_services("lightdm");
        complete_installation("Arch-XFCE-Desktop");
    }
    
    void install_lxqt_desktop() {
        std::cout << COLOR_CYAN << "Installing LXQt Desktop..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("lxqt xdg-utils sddm", "sddm")) return;
        
        enable_services("sddm");
        complete_installation("Arch-LXQt-Desktop");
    }
    
    void install_cinnamon_desktop() {
        std::cout << COLOR_CYAN << "Installing Cinnamon Desktop..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("cinnamon lightdm lightdm-gtk-greeter", "lightdm")) return;
        
        enable_services("lightdm");
        complete_installation("Arch-Cinnamon-Desktop");
    }
    
    void install_mate_desktop() {
        std::cout << COLOR_CYAN << "Installing MATE Desktop..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("mate mate-extra lightdm lightdm-gtk-greeter", "lightdm")) return;
        
        enable_services("lightdm");
        complete_installation("Arch-MATE-Desktop");
    }
    
    void install_budgie_desktop() {
        std::cout << COLOR_CYAN << "Installing Budgie Desktop..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("budgie-desktop gnome-control-center lightdm lightdm-gtk-greeter", "lightdm")) return;
        
        enable_services("lightdm");
        complete_installation("Arch-Budgie-Desktop");
    }
    
    void install_i3_wm() {
        std::cout << COLOR_CYAN << "Installing i3 Window Manager..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("i3-wm i3status i3lock dmenu rxvt-unicode lightdm lightdm-gtk-greeter", "lightdm")) return;
        
        enable_services("lightdm");
        complete_installation("Arch-i3-WM");
    }
    
    void install_sway_wm() {
        std::cout << COLOR_CYAN << "Installing Sway Window Manager..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("sway waybar wofi foot", "")) return;
        
        enable_services("");
        complete_installation("Arch-Sway-WM");
    }
    
    void install_hyprland_wm() {
        std::cout << COLOR_CYAN << "Installing Hyprland Window Manager..." << COLOR_RESET << std::endl;
        
        if (!setup_installation_environment()) return;
        
        if (!install_base_packages("hyprland waybar rofi foot", "")) return;
        
        enable_services("");
        complete_installation("Arch-Hyprland-WM");
    }
    
    // Show desktop selection menu with Arch Linux options
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
                case 0:
                    current_desktop_name = "Arch-TTY-Grub";
                    std::cout << COLOR_GREEN << "Desktop environment set to: Arch TTY Grub" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 1:
                    current_desktop_name = "GNOME-Desktop";
                    std::cout << COLOR_GREEN << "Desktop environment set to: GNOME Desktop" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 2:
                    current_desktop_name = "KDE-Plasma";
                    std::cout << COLOR_GREEN << "Desktop environment set to: KDE Plasma" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 3:
                    current_desktop_name = "XFCE-Desktop";
                    std::cout << COLOR_GREEN << "Desktop environment set to: XFCE Desktop" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 4:
                    current_desktop_name = "LXQt-Desktop";
                    std::cout << COLOR_GREEN << "Desktop environment set to: LXQt Desktop" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 5:
                    current_desktop_name = "Cinnamon-Desktop";
                    std::cout << COLOR_GREEN << "Desktop environment set to: Cinnamon Desktop" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 6:
                    current_desktop_name = "MATE-Desktop";
                    std::cout << COLOR_GREEN << "Desktop environment set to: MATE Desktop" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 7:
                    current_desktop_name = "Budgie-Desktop";
                    std::cout << COLOR_GREEN << "Desktop environment set to: Budgie Desktop" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 8:
                    current_desktop_name = "i3-WM";
                    std::cout << COLOR_GREEN << "Desktop environment set to: i3 Window Manager" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 9:
                    current_desktop_name = "Sway-WM";
                    std::cout << COLOR_GREEN << "Desktop environment set to: Sway Window Manager" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 10:
                    current_desktop_name = "Hyprland-WM";
                    std::cout << COLOR_GREEN << "Desktop environment set to: Hyprland Window Manager" << COLOR_RESET << std::endl;
                    save_configuration();
                    return;
                case 11:
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
            
            selected = show_menu(main_options, "claudemods Arch Linux distribution iso creator", selected);
            
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
        load_configuration();
        
        // EXTRACT FILES
        if (!extractRequiredFiles()) {
            std::cerr << COLOR_RED << "Failed to extract required files. Cannot continue." << COLOR_RESET << std::endl;
            return;
        }
        
        display_header();
        show_main_menu();
    }
};

int main() {
    ClaudemodsInstaller installer;
    installer.run();
    return 0;
}
