#ifndef CLAUDEMODS_H
#define CLAUDEMODS_H

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
    std::string extra_packages;
    std::string target_drive;
    std::string filesystem_type;

    struct termios oldt, newt;

    std::string getConfigFilePath() {
        return getCurrentDir() + "/configurationclaudemods.txt";
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

    std::string getCalamaresFolder() {
        return getCurrentDir() + "/calamares-claudemods";
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
            std::cout << COLOR_CYAN << "Creating calamares-claudemods folder..." << COLOR_RESET << std::endl;
            std::string createCmd = "sudo mkdir -p " + calamaresFolder;
            execute_command(createCmd);
        }
        std::string sourceDir = currentDir + "/calamares-per-distro/claudemods";
        std::vector<std::string> zipFiles = {
            "calamares-claudemods.zip",
            "claudemods.zip",
            "build-image-claudemods.zip"
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
        std::cout << "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗" << std::endl;
        std::cout << "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝" << std::endl;
        std::cout << "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░" << std::endl;
        std::cout << "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗" << std::endl;
        std::cout << "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝" << std::endl;
        std::cout << "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░" << std::endl;
        std::cout << COLOR_CYAN << "claudemods distribution iso creator Beta v1.01 03-06-2026" << COLOR_RESET << std::endl;
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
                if (title == "claudemods distribution iso creator") {
                    std::string setting_value;
                    switch(i) {
                        case 0: setting_value = target_drive.empty() ? "[Not Set]" : target_drive; break;
                        case 1: setting_value = "current directory as claudemods-distro"; break;
                        case 2: setting_value = new_username.empty() ? "[Not Set]" : new_username; break;
                        case 3: setting_value = root_password.empty() ? "[Not Set]" : root_password; break;
                        case 4: setting_value = user_password.empty() ? "[Not Set]" : user_password; break;
                        case 5: setting_value = timezone.empty() ? "[Not Set]" : timezone; break;
                        case 6: setting_value = keyboard_layout.empty() ? "[Not Set]" : keyboard_layout; break;
                        case 7: setting_value = "[Default GB]"; break;
                        case 8: setting_value = current_distro_name.empty() ? "[Not Set]" : current_distro_name; break;
                        case 9: setting_value = extra_packages.empty() ? "[Not Set]" : extra_packages; break;
                        default: setting_value = ""; break;
                    }
                    menu_line = options[i] + " " + setting_value;
                } else {
                    menu_line = options[i];
                }
                if (menu_line.length() > 80) {
                    menu_line = menu_line.substr(0, 77) + "...";
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

    bool create_directory(const std::string& path) {
        return system(("sudo mkdir -p " + path).c_str()) == 0;
    }

    void display_current_settings() {
        std::cout << COLOR_YELLOW << "\nCurrent Settings:" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Target Drive: " << COLOR_RESET
                  << (target_drive.empty() ? "[Not Set]" : target_drive) << std::endl;
        std::cout << COLOR_CYAN << "Filesystem: " << COLOR_RESET
                  << (filesystem_type.empty() ? "[Not Set]" : filesystem_type) << std::endl;
        std::cout << COLOR_CYAN << "Installation Path: " << COLOR_RESET << "current directory as claudemods-distro" << std::endl;
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
        std::cout << COLOR_CYAN << "Current Distro: " << COLOR_RESET
        << (current_distro_name.empty() ? "[Not Set]" : current_distro_name) << std::endl;
        std::cout << COLOR_CYAN << "Extra Packages: " << COLOR_RESET
        << (extra_packages.empty() ? "[Not Set]" : extra_packages) << std::endl;
        std::cout << std::endl;
    }

    bool setup_target_directory(const std::string& target_folder) {
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
        execute_command("sudo mkdir -p " + target_folder + "/etc/sysctl.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules " + target_folder + "/usr/lib/initcpio/udev/11-dm-initramfs.rules");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules /usr/lib/initcpio/udev/11-dm-initramfs.rules");
        execute_command("sudo cp -r " + currentDir + "/needed-files/80-gamecompatibility.conf " + target_folder + "/etc/sysctl.d/80-gamecompatibility.conf");
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");
        execute_command("sudo mkdir -p " + target_folder + "/usr/share/grub/themes");
        execute_command("sudo mkdir -p " + target_folder + "/usr/share/plymouth/themes");
        execute_command("sudo mkdir -p " + target_folder + "/usr/local/bin");
        execute_command("sudo mkdir -p " + target_folder + "/etc/systemd/system");
        execute_command("sudo mkdir -p " + target_folder + "/etc/sddm.conf.d");
        return true;
    }

    void setup_pacman_and_files(const std::string& target_folder) {
        std::string currentDir = getCurrentDir();
        execute_command("sudo cp -r " + currentDir + "/needed-files/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");
        execute_command("sudo unzip -o " + currentDir + "/needed-files/pacman.d.zip -d " + target_folder + "/etc/pacman.d");
        execute_command("sudo unzip -o " + currentDir + "/needed-files/pacman.d.zip -d /etc/pacman.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/pacman.conf " + target_folder + "/etc/pacman.conf");
        execute_command("sudo cp -r " + currentDir + "/needed-files/pacman.conf /etc/pacman.conf");
        execute_command("sudo pacman -Sy");
        execute_command("sudo pacman -S archlinux-keyring");
        execute_command("sudo pacman-key --populate");
        execute_command("sudo pacman-key --init");
        execute_command("sudo pacman -S arch-install-scripts");
    }

    bool verify_pacstrap_success(const std::string& target_folder) {
        struct stat info;
        std::string test_bin = target_folder + "/bin/bash";
        if (stat(test_bin.c_str(), &info) != 0) {
            std::cerr << COLOR_RED << "pacstrap failed! /bin/bash not found in target." << COLOR_RESET << std::endl;
            return false;
        }
        return true;
    }

    void setup_boot_directory(const std::string& target_folder) {
        execute_command("sudo mkdir -p " + target_folder + "/boot");
        execute_command("sudo mkdir -p " + target_folder + "/boot/grub");
        execute_command("sudo touch " + target_folder + "/boot/grub/grub.cfg.new");
    }

    void create_user_home_structure(const std::string& target_folder) {
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.config/fish");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.local/share");
        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");
    }

    void fix_user_places_xbel(const std::string& target_folder) {
        std::string cmd = "sudo ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sudo sed -i 's/spitfire/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }
    }

    void fix_user_places_xbel_apex(const std::string& target_folder) {
        std::string cmd = "sudo ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sudo sed -i 's/apex/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }
    }

    void install_calamares(const std::string& target_folder) {
        std::string currentDir = getCurrentDir();
        std::cout << COLOR_CYAN << "Installing Calamares installer and setting up iso ..." << COLOR_RESET << std::endl;
        execute_command("sudo cp " + currentDir + "/needed-files/kwalletrc " + target_folder + "/home/" + new_username + "/.config/kwalletrc");
        execute_command("sudo rm -rf " + target_folder + "/home/" + new_username + "/.local/share/kwalletd/*");
        execute_command("sudo cp -r " + currentDir + "/needed-files/wireless-regdom " + target_folder + "/etc/conf.d/wireless-regdom");
        execute_command("setfattr -n user.kde.fm.viewproperties#1 -v '[Dolphin]\\012Timestamp=2026,1,20,17,27,36.341\\012Version=4\\012\\012[Settings]\\012HiddenFilesShown=true' " + target_folder + "/home/" + new_username + "/.local/share/dolphin/view_properties/global");
        std::cout << COLOR_GREEN << "installation completed!" << COLOR_RESET << std::endl;
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
        std::cout << std::endl;
        std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
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
            execute_command("sudo chroot /mnt /bin/bash -c \""
                "modprobe efivarfs 2>/dev/null || true; "
                "mount -t efivarfs efivarfs /sys/firmware/efi/efivars 2>/dev/null || true; "
                "grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=GRUB --recheck; "
                "grub-mkconfig -o /boot/grub/grub.cfg; "
                "./opt/btrfsfstabcompressed.sh 2>/dev/null || genfstab -U / >> /etc/fstab; "
                "rm -rf /opt/btrfsfstabcompressed.sh 2>/dev/null; "
                "mkinitcpio -P\"");
        } else {
            execute_command("sudo chroot /mnt /bin/bash -c \""
                "modprobe efivarfs 2>/dev/null || true; "
                "mount -t efivarfs efivarfs /sys/firmware/efi/efivars 2>/dev/null || true; "
                "genfstab -U / >> /etc/fstab; "
                "grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=GRUB --recheck; "
                "grub-mkconfig -o /boot/grub/grub.cfg; "
                "mkinitcpio -P\"");
        }
    }

    void unmount_target() {
        std::cout << COLOR_CYAN << "Unmounting target filesystems..." << COLOR_RESET << std::endl;
        execute_command("sudo umount -R /mnt 2>/dev/null || true");
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
        
        // Prepare partitions and mount filesystem directly
        prepare_target_partitions();
        if (filesystem_type == "btrfs") {
            setup_btrfs_subvolumes();
        } else {
            setup_ext4_filesystem();
        }
        
        if (current_distro_name == "Spitfire-CKGE-Minimal") {
            install_spitfire_ckge_minimal();
        } else if (current_distro_name == "Spitfire-CKGE-Minimal-Dev") {
            install_spitfire_ckge_minimal_dev();
        } else if (current_distro_name == "Spitfire-CKGE-Full") {
            install_spitfire_ckge_full();
        } else if (current_distro_name == "Spitfire-CKGE-Full-Dev") {
            install_spitfire_ckge_full_dev();
        } else if (current_distro_name == "Spitfire-CKGE-Black-Full") {
            install_spitfire_ckge_black_full();
        } else if (current_distro_name == "Spitfire-CKGE-Black-Full-Dev") {
            install_spitfire_ckge_black_full_dev();
        } else if (current_distro_name == "Apex-CKGE-Minimal") {
            install_apex_ckge_minimal();
        } else if (current_distro_name == "Apex-CKGE-Minimal-Dev") {
            install_apex_ckge_minimal_dev();
        } else if (current_distro_name == "Apex-CKGE-Full") {
            install_apex_ckge_full();
        } else if (current_distro_name == "Apex-CKGE-Full-Dev") {
            install_apex_ckge_full_dev();
        } else {
            std::cout << COLOR_RED << "No valid distribution selected!" << COLOR_RESET << std::endl;
        }
    }

    void install_spitfire_ckge_minimal() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Minimal to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::cout << COLOR_CYAN << "Installing base system with pacstrap..." << COLOR_RESET << std::endl;
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop calamares-fix protonup-qt hhd adjustor hhd-ui sddm piper";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/term.service /mnt/etc/systemd/system/term.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-minimal.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/spitfire-minimal-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-minimal.zip");
        execute_command("sudo rm -rf " + currentDir + "/spitfire-minimal-v1.01.zip");
        fix_user_places_xbel(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_spitfire_ckge_minimal_dev() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Minimal Dev to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::cout << COLOR_CYAN << "Installing base system with pacstrap..." << COLOR_RESET << std::endl;
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-dev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/term.service /mnt/etc/systemd/system/term.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-minimal.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/spitfire-minimal-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-minimal.zip");
        execute_command("sudo rm -rf " + currentDir + "/spitfire-minimal-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksspitfire.sh");
        fix_user_places_xbel(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_spitfire_ckge_full() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Full to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-v1.01.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/spitfire-full-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-v1.01.zip");
        execute_command("sudo rm -rf " + currentDir + "/spitfire-full-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksspitfire.sh");
        fix_user_places_xbel(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_spitfire_ckge_full_dev() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Full Dev to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-v1.01.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/spitfire-full-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-v1.01.zip");
        execute_command("sudo rm -rf " + currentDir + "/spitfire-full-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksspitfire.sh");
        fix_user_places_xbel(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_spitfire_ckge_black_full() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Black Full to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-black-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-CKGBE-v1.01.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-CKGBE-v1.01.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/spitfire-full-black-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-CKGBE-v1.01.zip");
        execute_command("sudo rm -rf " + currentDir + "/spitfire-full-black-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksspitfire.sh");
        fix_user_places_xbel(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_spitfire_ckge_black_full_dev() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Black Full Dev to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-black-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-CKGBE-v1.01.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-CKGBE-v1.01.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/spitfire-full-black-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksspitfire.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/SpitFireLogin /mnt/usr/share/sddm/themes/SpitFireLogin");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-CKGBE-v1.01.zip");
        execute_command("sudo rm -rf " + currentDir + "/spitfire-full-black-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksspitfire.sh");
        fix_user_places_xbel(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_apex_ckge_minimal() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Minimal to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop calamares-fix protonup-qt hhd adjustor hhd-ui sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/term.service /mnt/etc/systemd/system/term.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-minimal.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/apex-minimal-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksapex.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-minimal.zip");
        execute_command("sudo rm -rf " + currentDir + "/apex-minimal-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksapex.sh");
        fix_user_places_xbel_apex(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_apex_ckge_minimal_dev() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Minimal Dev to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-dev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm piper";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/term.sh /mnt/usr/local/bin/term.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/term.service /mnt/etc/systemd/system/term.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-minimal.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-minimal.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/apex-minimal-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksapex.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-minimal.zip");
        execute_command("sudo rm -rf " + currentDir + "/apex-minimal-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksapex.sh");
        fix_user_places_xbel_apex(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_apex_ckge_full() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Full to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-full calamares-fix lutris protonup-qt hhd adjustor hhd-ui obs-studio sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-v1.01.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/apex-full-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksapex.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-v1.01.zip");
        execute_command("sudo rm -rf " + currentDir + "/apex-full-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksapex.sh");
        fix_user_places_xbel_apex(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
    }

    void install_apex_ckge_full_dev() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Full Dev to " << target_drive << "..." << COLOR_RESET << std::endl;
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        std::string pacstrap_cmd = "sudo pacstrap /mnt claudemods-desktop-fulldev calamares-fix lutris protonup-qt hhd adjustor hhd-ui sddm";
        if (!extra_packages.empty()) { pacstrap_cmd += " " + extra_packages; }
        execute_command(pacstrap_cmd);
        if (!verify_pacstrap_success(target_folder)) return;
        setup_boot_directory(target_folder);
        mount_system_dirs();
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable NetworkManager\"");
        apply_timezone_keyboard_settings();
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub /mnt/etc/default/grub");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/grub.cfg /mnt/boot/grub/grub.cfg");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/cachyos /mnt/usr/share/grub/themes");
        execute_command("sudo chroot /mnt /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation /mnt/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/termfull.sh /mnt/usr/local/bin/termfull.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/termfull.service /mnt/etc/systemd/system/termfull.service");
        execute_command("sudo chroot /mnt /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");
        create_user();
        create_user_home_structure(target_folder);
        execute_command("cd /mnt");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-v1.01.zip -d /mnt/opt");
        execute_command("sudo unzip -o " + currentDir + "/apex-full-v1.01.zip -d /mnt/home/" + new_username + "/");
        execute_command("sudo mkdir -p /mnt/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/kde_settings.conf /mnt/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/apex-ckge-minimal/tweaksapex.sh /mnt/opt/tweaksapex.sh");
        execute_command("sudo chmod +x /mnt/opt/tweaksapex.sh");
        execute_command("sudo chroot /mnt /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/konsolerc /mnt/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/ApexLogin2 /mnt/usr/share/sddm/themes/ApexLogin2");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.colorscheme /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/apex-ckge-minimal/claudemods-cyan.profile /mnt/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + currentDir + "/Arch-Systemtool-v1.01.zip");
        execute_command("sudo rm -rf " + currentDir + "/apex-full-v1.01.zip");
        execute_command("sudo rm -rf /mnt/opt/tweaksapex.sh");
        fix_user_places_xbel_apex(target_folder);
        install_calamares(target_folder);
        unmount_system_dirs();
        install_grub();
        std::cout << COLOR_GREEN << "\nInstallation complete! System installed to " << target_drive << COLOR_RESET << std::endl;
        post_install_menu();
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

    void set_wireless_regdom() {
        std::string currentDir = getCurrentDir();
        std::string wireless_regdom_file = currentDir + "/needed-files/wireless-regdom";
        std::cout << COLOR_CYAN << "Opening wireless regulatory domain file for editing..." << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "File: " << wireless_regdom_file << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "Use Ctrl+X to exit nano after editing" << COLOR_RESET << std::endl;
        std::string nano_cmd = "nano " + wireless_regdom_file;
        execute_command(nano_cmd);
        std::cout << COLOR_GREEN << "Wireless regulatory domain file updated." << COLOR_RESET << std::endl;
    }

    void set_extra_packages() {
        extra_packages = get_input("Enter extra packages (space separated): ");
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
        if (current_distro_name.empty()) {
            std::cout << COLOR_RED << "Error: No distribution selected!" << COLOR_RESET << std::endl;
            return false;
        }
        return true;
    }

    void show_distro_selection() {
        std::vector<std::string> distro_options = {
            "Install Spitfire CKGE Minimal",
            "Install Spitfire CKGE Minimal Dev",
            "Install Spitfire CKGE Full",
            "Install Spitfire CKGE Full Dev",
            "Install Spitfire CKGE Black Full",
            "Install Spitfire CKGE Black Full Dev",
            "Install Apex CKGE Minimal",
            "Install Apex CKGE Minimal Dev",
            "Install Apex CKGE Full",
            "Install Apex CKGE Full Dev",
            "Back to Main Menu"
        };
        int selected = 0;
        while (true) {
            selected = show_menu(distro_options, "Select Distribution to Install", selected);
            switch(selected) {
                case 0: current_distro_name = "Spitfire-CKGE-Minimal"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Spitfire CKGE Minimal selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 1: current_distro_name = "Spitfire-CKGE-Minimal-Dev"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Spitfire CKGE Minimal Dev selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 2: current_distro_name = "Spitfire-CKGE-Full"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Spitfire CKGE Full selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 3: current_distro_name = "Spitfire-CKGE-Full-Dev"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Spitfire CKGE Full Dev selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 4: current_distro_name = "Spitfire-CKGE-Black-Full"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Spitfire CKGE Black Full selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 5: current_distro_name = "Spitfire-CKGE-Black-Full-Dev"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Spitfire CKGE Black Full Dev selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 6: current_distro_name = "Apex-CKGE-Minimal"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Apex CKGE Minimal selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 7: current_distro_name = "Apex-CKGE-Minimal-Dev"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Apex CKGE Minimal Dev selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 8: current_distro_name = "Apex-CKGE-Full"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Apex CKGE Full selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 9: current_distro_name = "Apex-CKGE-Full-Dev"; saveConfiguration();
                    std::cout << COLOR_GREEN << "Apex CKGE Full Dev selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl; return;
                case 10: return;
            }
            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

    void show_main_menu() {
        std::vector<std::string> main_options = {
            "Setup Bootloader and Drive: ",
            "Installation Path: ",
            "Set Username",
            "Set Root Password",
            "Set User Password",
            "Set Timezone",
            "Set Keyboard Layout",
            "Set Wireless Regdom",
            "Select Distro to Install",
            "Install Extra Packages",
            "Start Installation",
            "Exit"
        };
        int selected = 0;
        while (true) {
            system("clear");
            display_header();
            display_current_settings();
            selected = show_menu(main_options, "claudemods distribution iso creator", selected);
            switch(selected) {
                case 0: setup_bootloader_and_drive(); break;
                case 1: break;
                case 2: set_username(); break;
                case 3: set_root_password(); break;
                case 4: set_user_password(); break;
                case 5: set_timezone(); break;
                case 6: set_keyboard_layout(); break;
                case 7: set_wireless_regdom(); break;
                case 8: show_distro_selection(); break;
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

#endif // CLAUDEMODS_H
