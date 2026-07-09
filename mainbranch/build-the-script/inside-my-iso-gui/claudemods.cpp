// claudemods_installer.cpp - Complete Qt6 GUI Application
// ALL original code preserved, ALL functions intact, ALL commands unchanged

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QScrollArea>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QInputDialog>
#include <QSplitter>
#include <QProgressBar>
#include <QListWidget>
#include <QDateTime>
#include <QTextCursor>
#include <QRegularExpression>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QHeaderView>
#include <QTimer>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <limits>
#include <termios.h>

// Color definitions - PRESERVED EXACTLY
const std::string COLOR_CYAN = "\033[38;2;0;255;255m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_ORANGE = "\033[38;5;208m";
const std::string COLOR_PURPLE = "\033[38;5;93m";
const std::string COLOR_RESET = "\033[0m";

class ClaudemodsInstallerQt : public QMainWindow
{
    Q_OBJECT

private:
    // ALL original member variables preserved
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

    // Qt UI elements
    QTextEdit *m_logOutput;
    QLabel *m_settingsLabel;
    QLabel *m_statusLabel;
    QProgressBar *m_statusProgress;
    QProcess *m_process;
    bool m_busy;

public:
    ClaudemodsInstallerQt(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        new_username = "";
        root_password = "";
        user_password = "";
        timezone = "";
        keyboard_layout = "";
        current_distro_name = "";
        extra_packages = "";
        target_drive = "";
        filesystem_type = "";
        m_process = nullptr;
        m_busy = false;
        
        setupUI();
        loadConfiguration();
        extractRequiredFiles();
        display_current_settings();
        
        addLog("░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗", "#ff0000");
        addLog("██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝", "#ff0000");
        addLog("██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░", "#ff0000");
        addLog("██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗", "#ff0000");
        addLog("╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝", "#ff0000");
        addLog("░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░", "#ff0000");
        addLog("claudemods distribution iso creator Beta v1.01 06-06-2026", "#00ffff");
        addLog("Qt6 GUI Edition - All original functionality preserved", "#00ffff");
        addLog("", "#ffffff");
    }

    ~ClaudemodsInstallerQt()
    {
        if (m_process) {
            m_process->kill();
            m_process->waitForFinished();
        }
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (m_busy) {
            QMessageBox::warning(this, "Process Running", 
                "An installation process is running.\nPlease wait for it to complete before closing.");
            event->ignore();
        } else {
            event->accept();
        }
    }

private:
    void setupUI()
    {
        setWindowTitle("claudemods distribution iso creator Beta v1.01");
        setMinimumSize(1400, 900);
        setStyleSheet("QMainWindow { background-color: #1a1a1a; }");
        
        // Menu Bar
        QMenuBar *menuBar = new QMenuBar();
        menuBar->setStyleSheet("QMenuBar { background-color: #1a1a1a; color: #00ffff; padding: 5px; } QMenuBar::item { padding: 5px 15px; } QMenuBar::item:selected { background-color: #333; } QMenu { background-color: #222; color: #00ffff; border: 1px solid #555; } QMenu::item { padding: 8px 30px; } QMenu::item:selected { background-color: #444; }");
        
        QMenu *fileMenu = menuBar->addMenu("File");
        QAction *loadAction = fileMenu->addAction("Load Configuration");
        connect(loadAction, &QAction::triggered, [=]() { loadConfiguration(); display_current_settings(); });
        QAction *saveAction = fileMenu->addAction("Save Configuration");
        connect(saveAction, &QAction::triggered, [=]() { saveConfiguration(); });
        fileMenu->addSeparator();
        QAction *exitAction = fileMenu->addAction("Exit");
        connect(exitAction, &QAction::triggered, this, &ClaudemodsInstallerQt::close);
        
        QMenu *helpMenu = menuBar->addMenu("Help");
        QAction *aboutAction = helpMenu->addAction("About");
        connect(aboutAction, &QAction::triggered, [=]() {
            QMessageBox::about(this, "About", "claudemods distribution iso creator\nBeta v1.01 - Qt6 Edition\n\nOriginal CLI version: 06-06-2026");
        });
        
        setMenuBar(menuBar);
        
        // Status Bar
        QStatusBar *status = statusBar();
        status->setStyleSheet("QStatusBar { background-color: #1a1a1a; color: #00ffff; border-top: 1px solid #333; padding: 5px; }");
        status->showMessage("Ready | claudemods distribution iso creator v1.01");
        
        QWidget *centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(5, 5, 5, 5);
        
        // LEFT PANEL - ALL GUI BUTTONS FOR EVERY FUNCTION
        QWidget *leftPanel = new QWidget();
        leftPanel->setFixedWidth(400);
        leftPanel->setStyleSheet("QWidget { background-color: #222; border-radius: 10px; }");
        QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setSpacing(8);
        
        // ASCII Art Header
        QLabel *headerLabel = new QLabel();
        headerLabel->setStyleSheet("color: #ff0000; font-family: 'Courier New', monospace; font-size: 6px; padding: 10px; background-color: #111; border-radius: 8px;");
        headerLabel->setText(
            "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗\n"
            "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝\n"
            "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░\n"
            "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗\n"
            "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝\n"
            "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░"
        );
        leftLayout->addWidget(headerLabel);
        
        QLabel *versionLabel = new QLabel("claudemods distribution iso creator\nBeta v1.01 - Qt6 Edition");
        versionLabel->setStyleSheet("color: #00ffff; font-size: 11px; padding: 5px;");
        versionLabel->setAlignment(Qt::AlignCenter);
        leftLayout->addWidget(versionLabel);
        
        // Settings display group
        QGroupBox *settingsGroup = new QGroupBox("Current Settings");
        settingsGroup->setStyleSheet("QGroupBox { color: #ffff00; border: 2px solid #555; border-radius: 8px; margin-top: 15px; font-weight: bold; font-size: 13px; padding-top: 10px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
        QVBoxLayout *settingsLayout = new QVBoxLayout(settingsGroup);
        m_settingsLabel = new QLabel();
        m_settingsLabel->setStyleSheet("color: #00ffff; font-size: 11px; font-family: 'Courier New', monospace; padding: 5px;");
        m_settingsLabel->setWordWrap(true);
        settingsLayout->addWidget(m_settingsLabel);
        leftLayout->addWidget(settingsGroup);
        
        // Scroll area for buttons
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; } QScrollBar:vertical { background: #111; width: 10px; } QScrollBar::handle:vertical { background: #333; border-radius: 5px; }");
        
        QWidget *scrollWidget = new QWidget();
        QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
        scrollLayout->setSpacing(6);
        
        // ALL GUI BUTTONS FOR EVERY FUNCTION
        auto addButton = [&](const QString &text, const QString &color, auto slot) {
            QPushButton *btn = new QPushButton(text);
            btn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; font-weight: bold; padding: 12px 15px; border: none; border-radius: 8px; font-size: 13px; text-align: left; } QPushButton:hover { opacity: 0.8; } QPushButton:disabled { background-color: #555; color: #888; }").arg(color));
            btn->setMinimumHeight(42);
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, &QPushButton::clicked, this, slot);
            scrollLayout->addWidget(btn);
        };
        
        addButton("💾 Setup Bootloader and Drive", "#ff6600", &ClaudemodsInstallerQt::setup_bootloader_and_drive);
        addButton("👤 Set Username", "#0088ff", &ClaudemodsInstallerQt::set_username);
        addButton("🔑 Set Root Password", "#cc0000", &ClaudemodsInstallerQt::set_root_password);
        addButton("🔒 Set User Password", "#cc6600", &ClaudemodsInstallerQt::set_user_password);
        addButton("🕐 Set Timezone", "#00aa88", &ClaudemodsInstallerQt::set_timezone);
        addButton("⌨️ Set Keyboard Layout", "#888800", &ClaudemodsInstallerQt::set_keyboard_layout);
        addButton("📡 Set Wireless Regdom", "#aa00aa", &ClaudemodsInstallerQt::set_wireless_regdom);
        addButton("📦 Select Distro to Install", "#ff00ff", &ClaudemodsInstallerQt::show_distro_selection);
        addButton("📥 Install Extra Packages", "#0088cc", &ClaudemodsInstallerQt::set_extra_packages);
        addButton("🚀 START INSTALLATION", "#00ff00", &ClaudemodsInstallerQt::start_installation);
        addButton("❌ Exit", "#ff0000", &ClaudemodsInstallerQt::close);
        
        scrollLayout->addStretch();
        scrollArea->setWidget(scrollWidget);
        leftLayout->addWidget(scrollArea);
        
        // RIGHT PANEL - Konsole log output
        QWidget *rightPanel = new QWidget();
        rightPanel->setStyleSheet("QWidget { background-color: #111; border-radius: 10px; }");
        QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
        
        QHBoxLayout *logHeaderLayout = new QHBoxLayout();
        QLabel *logIcon = new QLabel("🖥️");
        logIcon->setStyleSheet("font-size: 20px;");
        QLabel *logLabel = new QLabel("KONSOLE LOG OUTPUT");
        logLabel->setStyleSheet("color: #00ffff; font-weight: bold; font-size: 16px; font-family: 'Courier New', monospace;");
        logHeaderLayout->addWidget(logIcon);
        logHeaderLayout->addWidget(logLabel);
        logHeaderLayout->addStretch();
        QPushButton *clearLogBtn = new QPushButton("Clear Log");
        clearLogBtn->setStyleSheet("QPushButton { background-color: #555; color: white; padding: 8px 15px; border: none; border-radius: 5px; } QPushButton:hover { background-color: #777; }");
        connect(clearLogBtn, &QPushButton::clicked, [=]() { m_logOutput->clear(); });
        logHeaderLayout->addWidget(clearLogBtn);
        rightLayout->addLayout(logHeaderLayout);
        
        m_logOutput = new QTextEdit();
        m_logOutput->setReadOnly(true);
        m_logOutput->setStyleSheet(
            "QTextEdit { background-color: #000000; color: #00ff00; border: 2px solid #333; border-radius: 8px; "
            "font-family: 'Courier New', monospace; font-size: 12px; padding: 10px; selection-background-color: #004400; }"
            "QScrollBar:vertical { background: #111; width: 12px; }"
            "QScrollBar::handle:vertical { background: #333; border-radius: 6px; min-height: 20px; }"
        );
        rightLayout->addWidget(m_logOutput);
        
        // Status bar at bottom of right panel
        QHBoxLayout *statusLayout = new QHBoxLayout();
        m_statusLabel = new QLabel("Ready");
        m_statusLabel->setStyleSheet("color: #aaaaaa; font-size: 11px;");
        m_statusProgress = new QProgressBar();
        m_statusProgress->setStyleSheet("QProgressBar { border: 1px solid #333; border-radius: 3px; text-align: center; height: 16px; max-width: 200px; } QProgressBar::chunk { background-color: #00aa00; border-radius: 2px; }");
        m_statusProgress->setVisible(false);
        statusLayout->addWidget(m_statusLabel);
        statusLayout->addStretch();
        statusLayout->addWidget(m_statusProgress);
        rightLayout->addLayout(statusLayout);
        
        // Splitter
        QSplitter *splitter = new QSplitter(Qt::Horizontal);
        splitter->addWidget(leftPanel);
        splitter->addWidget(rightPanel);
        splitter->setStretchFactor(0, 0);
        splitter->setStretchFactor(1, 1);
        splitter->setSizes({400, 1000});
        splitter->setStyleSheet("QSplitter::handle { background-color: #333; width: 3px; }");
        mainLayout->addWidget(splitter);
    }

    void addLog(const QString &message, const QString &color = "#00ff00")
    {
        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
        QString formattedMsg = QString("<span style='color: %1;'>[%2] %3</span>").arg(color, timestamp, message.toHtmlEscaped());
        m_logOutput->append(formattedMsg);
        
        QTextCursor cursor = m_logOutput->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_logOutput->setTextCursor(cursor);
        
        m_statusLabel->setText(message);
        QApplication::processEvents();
    }

    // ========================================================================
    // ALL ORIGINAL METHODS PRESERVED EXACTLY - JUST WITH GUI OUTPUT
    // ========================================================================

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
            addLog("Configuration saved to " + QString::fromStdString(configFile), "#00ff00");
        } else {
            addLog("Failed to save configuration to " + QString::fromStdString(configFile), "#ff0000");
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
            addLog("Configuration loaded from " + QString::fromStdString(configFile), "#00ff00");
        } else {
            addLog("No existing configuration found. Starting with default settings.", "#ffff00");
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
        addLog("Checking for required folders...", "#00ffff");
        std::string currentDir = getCurrentDir();
        std::string calamaresFolder = getCalamaresFolder();
        bool buildImageExists = directoryExists(calamaresFolder + "/build-image-arch-img");
        bool calamaresFilesExists = directoryExists(calamaresFolder + "/calamares-files");
        bool workingHooksExists = directoryExists(calamaresFolder + "/working-hooks-btrfs-ext4");
        if (buildImageExists && calamaresFilesExists && workingHooksExists) {
            addLog("All required folders already exist.", "#00ff00");
            return true;
        }
        if (!directoryExists(calamaresFolder)) {
            addLog("Creating calamares-claudemods folder...", "#00ffff");
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
                addLog("Extracting " + QString::fromStdString(zipFile) + "...", "#00ffff");
                std::string extractCmd = "sudo unzip -q " + sourcePath + " -d " + calamaresFolder + " >/dev/null 2>&1";
                execute_command(extractCmd);
                addLog("Done.", "#00ff00");
            }
        }
        addLog("Extraction process completed.", "#00ff00");
        return true;
    }

    void display_header() {
        addLog("░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗", "#ff0000");
        addLog("██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝", "#ff0000");
        addLog("██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░", "#ff0000");
        addLog("██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗", "#ff0000");
        addLog("╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝", "#ff0000");
        addLog("░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░", "#ff0000");
        addLog("claudemods distribution iso creator Beta v1.01 06-06-2026", "#00ffff");
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

    int execute_command(const std::string& cmd) {
        addLog("$ " + QString::fromStdString(cmd), "#888888");
        
        QProcess process;
        process.start("bash", QStringList() << "-c" << QString::fromStdString(cmd));
        process.waitForFinished(-1);
        
        QString output = process.readAllStandardOutput().trimmed();
        QString error = process.readAllStandardError().trimmed();
        
        if (!output.isEmpty()) {
            for (const QString &line : output.split('\n')) {
                if (!line.trimmed().isEmpty())
                    addLog("  " + line.trimmed(), "#cccccc");
            }
        }
        if (!error.isEmpty()) {
            for (const QString &line : error.split('\n')) {
                if (!line.trimmed().isEmpty())
                    addLog("  [ERR] " + line.trimmed(), "#ff6666");
            }
        }
        
        return process.exitCode();
    }

    void display_current_settings() {
        QString settings;
        settings += "\nCurrent Settings:\n";
        settings += "Target Drive: " + QString::fromStdString(target_drive.empty() ? "[Not Set]" : target_drive) + "\n";
        settings += "Filesystem: " + QString::fromStdString(filesystem_type.empty() ? "[Not Set]" : filesystem_type) + "\n";
        settings += "Username: " + QString::fromStdString(new_username.empty() ? "[Not Set]" : new_username) + "\n";
        settings += "Root Password: " + QString::fromStdString(root_password.empty() ? "[Not Set]" : "********") + "\n";
        settings += "User Password: " + QString::fromStdString(user_password.empty() ? "[Not Set]" : "********") + "\n";
        settings += "Timezone: " + QString::fromStdString(timezone.empty() ? "[Not Set]" : timezone) + "\n";
        settings += "Keyboard Layout: " + QString::fromStdString(keyboard_layout.empty() ? "[Not Set]" : keyboard_layout) + "\n";
        settings += "Current Distro: " + QString::fromStdString(current_distro_name.empty() ? "[Not Set]" : current_distro_name) + "\n";
        settings += "Extra Packages: " + QString::fromStdString(extra_packages.empty() ? "[Not Set]" : extra_packages) + "\n";
        
        m_settingsLabel->setText(settings);
    }

    bool setup_target_directory(const std::string& target_folder) {
        std::string currentDir = getCurrentDir();
        addLog("Creating target directory: " + QString::fromStdString(target_folder), "#00ffff");
        execute_command("sudo mkdir -p " + target_folder);
        struct stat info;
        if (stat(target_folder.c_str(), &info) != 0) {
            addLog("Failed to create target directory: " + QString::fromStdString(target_folder), "#ff0000");
            return false;
        }
        if (!(info.st_mode & S_IFDIR)) {
            addLog("Target path is not a directory: " + QString::fromStdString(target_folder), "#ff0000");
            return false;
        }
        addLog("Target directory created successfully!", "#00ff00");
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
    }

    bool verify_pacstrap_success(const std::string& target_folder) {
        struct stat info;
        std::string test_bin = target_folder + "/bin/bash";
        if (stat(test_bin.c_str(), &info) != 0) {
            addLog("pacstrap failed! /bin/bash not found in target.", "#ff0000");
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
        addLog("Installing Calamares installer and setting up iso ...", "#00ffff");
        execute_command("sudo cp " + currentDir + "/needed-files/kwalletrc " + target_folder + "/home/" + new_username + "/.config/kwalletrc");
        execute_command("sudo rm -rf " + target_folder + "/home/" + new_username + "/.local/share/kwalletd/*");
        execute_command("sudo cp -r " + currentDir + "/needed-files/wireless-regdom " + target_folder + "/etc/conf.d/wireless-regdom");
        execute_command("setfattr -n user.kde.fm.viewproperties#1 -v '[Dolphin]\\012Timestamp=2026,1,20,17,27,36.341\\012Version=4\\012\\012[Settings]\\012HiddenFilesShown=true' " + target_folder + "/home/" + new_username + "/.local/share/dolphin/view_properties/global");
        addLog("installation completed!", "#00ff00");
    }

    // GUI dialog for setup_bootloader_and_drive
    void setup_bootloader_and_drive() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Setup Bootloader and Drive");
        dialog->setMinimumSize(900, 700);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
        
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════════════════════════╗\n║ Setup Bootloader and Drive                                                      ║\n╚══════════════════════════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        mainLayout->addWidget(titleLabel);
        
        // Current settings
        QGroupBox *currentGroup = new QGroupBox("Current Drive Settings:");
        currentGroup->setStyleSheet("QGroupBox { color: #ffff00; border: 2px solid #555; border-radius: 8px; margin-top: 15px; font-weight: bold; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
        QGridLayout *currentGrid = new QGridLayout(currentGroup);
        
        QLabel *driveLabel = new QLabel("Target Drive:");
        driveLabel->setStyleSheet("color: #00ffff;");
        QLabel *driveValue = new QLabel(QString::fromStdString(target_drive.empty() ? "[Not Set]" : target_drive));
        driveValue->setStyleSheet("color: #00ff00; font-weight: bold;");
        
        QLabel *fsLabel = new QLabel("Filesystem:");
        fsLabel->setStyleSheet("color: #00ffff;");
        QLabel *fsValue = new QLabel(QString::fromStdString(filesystem_type.empty() ? "[Not Set]" : filesystem_type));
        fsValue->setStyleSheet("color: #00ff00; font-weight: bold;");
        
        currentGrid->addWidget(driveLabel, 0, 0);
        currentGrid->addWidget(driveValue, 0, 1);
        currentGrid->addWidget(fsLabel, 1, 0);
        currentGrid->addWidget(fsValue, 1, 1);
        mainLayout->addWidget(currentGroup);
        
        // Available drives table
        QGroupBox *drivesGroup = new QGroupBox("Available drives:");
        drivesGroup->setStyleSheet("QGroupBox { color: #00ffff; border: 2px solid #555; border-radius: 8px; margin-top: 15px; font-weight: bold; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
        QVBoxLayout *drivesLayout = new QVBoxLayout(drivesGroup);
        
        QTableWidget *driveTable = new QTableWidget();
        driveTable->setColumnCount(3);
        driveTable->setHorizontalHeaderLabels({"NAME", "SIZE", "MODEL"});
        driveTable->setStyleSheet("QTableWidget { background-color: #2d2d2d; color: #ffff00; border: 1px solid #555; } QTableWidget::item { padding: 5px; } QHeaderView::section { background-color: #333; color: #00ffff; font-weight: bold; padding: 5px; }");
        driveTable->horizontalHeader()->setStretchLastSection(true);
        driveTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        driveTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        driveTable->verticalHeader()->setVisible(false);
        
        std::string cmd = "lsblk -d -o NAME,SIZE,MODEL | grep -v 'loop\\|sr0\\|zram'";
        std::string result = exec_cmd(cmd.c_str());
        std::istringstream stream(result);
        std::string line;
        int row = 0;
        std::getline(stream, line);
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
                
                driveTable->insertRow(row);
                driveTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString("/dev/" + name)));
                driveTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(size_str)));
                driveTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(model)));
                row++;
            }
        }
        drivesLayout->addWidget(driveTable);
        mainLayout->addWidget(drivesGroup);
        
        // Warning
        QLabel *warningLabel = new QLabel("⚠️ WARNING: The selected drive will be COMPLETELY ERASED!");
        warningLabel->setStyleSheet("color: #ffff00; font-weight: bold; font-size: 14px; background-color: #330000; padding: 10px; border: 2px solid #ff0000; border-radius: 5px;");
        mainLayout->addWidget(warningLabel);
        
        // Drive input
        QGroupBox *inputGroup = new QGroupBox("Enter target drive (e.g., /dev/sda):");
        inputGroup->setStyleSheet("QGroupBox { color: #ff6600; border: 2px solid #555; border-radius: 8px; margin-top: 15px; font-weight: bold; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
        QHBoxLayout *inputLayout = new QHBoxLayout(inputGroup);
        QLineEdit *driveInput = new QLineEdit();
        driveInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; } QLineEdit:focus { border-color: #00ffff; }");
        driveInput->setPlaceholderText("/dev/sda");
        driveInput->setText(QString::fromStdString(target_drive));
        QPushButton *selectBtn = new QPushButton("Use Selected");
        selectBtn->setStyleSheet("QPushButton { background-color: #0088ff; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; } QPushButton:hover { opacity: 0.8; }");
        connect(selectBtn, &QPushButton::clicked, [=]() {
            int selectedRow = driveTable->currentRow();
            if (selectedRow >= 0) {
                driveInput->setText(driveTable->item(selectedRow, 0)->text());
            }
        });
        inputLayout->addWidget(driveInput);
        inputLayout->addWidget(selectBtn);
        mainLayout->addWidget(inputGroup);
        
        // Filesystem selection
        QGroupBox *fsGroup = new QGroupBox("Select filesystem type:");
        fsGroup->setStyleSheet("QGroupBox { color: #aa00aa; border: 2px solid #555; border-radius: 8px; margin-top: 15px; font-weight: bold; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
        QVBoxLayout *fsLayout = new QVBoxLayout(fsGroup);
        
        QComboBox *fsCombo = new QComboBox();
        fsCombo->setStyleSheet("QComboBox { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; } QComboBox::drop-down { border: none; } QComboBox QAbstractItemView { background-color: #2d2d2d; color: #00ff00; selection-background-color: #444; }");
        fsCombo->addItem("Btrfs (with subvolumes, compression, snapshots support)");
        fsCombo->addItem("Ext4 (standard filesystem)");
        if (filesystem_type == "btrfs") fsCombo->setCurrentIndex(0);
        else if (filesystem_type == "ext4") fsCombo->setCurrentIndex(1);
        fsLayout->addWidget(fsCombo);
        
        QLabel *btrfsInfo = new QLabel("Btrfs subvolumes will be created: @, @home, @root, @srv, @cache, @tmp, @log\nCompression: zstd level 22");
        btrfsInfo->setStyleSheet("color: #00ffff;");
        btrfsInfo->setVisible(fsCombo->currentIndex() == 0);
        fsLayout->addWidget(btrfsInfo);
        
        connect(fsCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int idx) {
            btrfsInfo->setVisible(idx == 0);
        });
        mainLayout->addWidget(fsGroup);
        
        // Buttons
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *confirmBtn = new QPushButton("Confirm & Save");
        confirmBtn->setStyleSheet("QPushButton { background-color: #ff6600; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; } QPushButton:hover { opacity: 0.8; }");
        confirmBtn->setMinimumHeight(45);
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; } QPushButton:hover { opacity: 0.8; }");
        cancelBtn->setMinimumHeight(45);
        btnLayout->addWidget(confirmBtn);
        btnLayout->addWidget(cancelBtn);
        mainLayout->addLayout(btnLayout);
        
        connect(confirmBtn, &QPushButton::clicked, [=]() {
            std::string drive_input = driveInput->text().toStdString();
            if (drive_input.empty()) {
                addLog("No drive selected. Keeping current setting.", "#ffff00");
            } else if (!is_block_device(drive_input)) {
                addLog("Error: " + QString::fromStdString(drive_input) + " is not a valid block device!", "#ff0000");
            } else {
                target_drive = drive_input;
                addLog("Target drive set to: " + QString::fromStdString(target_drive), "#00ff00");
            }
            
            if (!target_drive.empty()) {
                if (fsCombo->currentIndex() == 0) {
                    filesystem_type = "btrfs";
                    addLog("Filesystem set to: Btrfs", "#00ff00");
                    addLog("Btrfs subvolumes will be created: @, @home, @root, @srv, @cache, @tmp, @log", "#00ffff");
                    addLog("Compression: zstd level 22", "#00ffff");
                } else {
                    filesystem_type = "ext4";
                    addLog("Filesystem set to: Ext4", "#00ff00");
                }
            }
            
            if (!target_drive.empty() && !filesystem_type.empty()) {
                addLog("═══════════════════════════════════════", "#ff6600");
                addLog("  Drive Configuration Summary:", "#ff6600");
                addLog("  Drive: " + QString::fromStdString(target_drive), "#ff6600");
                addLog("  Partition 1: " + QString::fromStdString(target_drive) + "1 (EFI - FAT32, 550MB)", "#ff6600");
                addLog("  Partition 2: " + QString::fromStdString(target_drive) + "2 (Root - " + QString::fromStdString(filesystem_type == "btrfs" ? "Btrfs" : "Ext4") + ")", "#ff6600");
                addLog("  Bootloader: GRUB (UEFI)", "#ff6600");
                addLog("═══════════════════════════════════════", "#ff6600");
                addLog("WARNING: ALL DATA ON " + QString::fromStdString(target_drive) + " WILL BE DESTROYED!", "#ff0000");
            }
            
            saveConfiguration();
            display_current_settings();
            dialog->accept();
        });
        
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void prepare_target_partitions() {
        addLog("Preparing target partitions on " + QString::fromStdString(target_drive) + "...", "#00ffff");
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
            addLog("Error: Failed to create partitions", "#ff0000");
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
        addLog("Setting up Btrfs subvolumes with zstd:22 compression...", "#00ffff");
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
        addLog("Setting up Ext4 filesystem...", "#00ffff");
        execute_command("sudo mount " + root_part + " /mnt");
        execute_command("sudo mkdir -p /mnt/{home,boot/efi,etc,usr,var,proc,sys,dev,tmp,run}");
    }

    void install_grub() {
        addLog("Installing GRUB bootloader...", "#00ffff");
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
                "grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=Cachyos --recheck; "
                "grub-mkconfig -o /boot/grub/grub.cfg; "
                "./opt/btrfsfstabcompressed.sh 2>/dev/null; "
                "rm -rf /opt/btrfsfstabcompressed.sh 2>/dev/null; "
                "rm -rf /var/cache/pacman/pkg/*; "
                "mkinitcpio -P\"");
        } else {
            execute_command("sudo chroot /mnt /bin/bash -c \""
                "modprobe efivarfs 2>/dev/null || true; "
                "mount -t efivarfs efivarfs /sys/firmware/efi/efivars 2>/dev/null || true; "
                "genfstab -U / >> /etc/fstab; "
                "grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=cachyos --recheck; "
                "grub-mkconfig -o /boot/grub/grub.cfg; "
                "rm -rf /var/cache/pacman/pkg/*; "
                "mkinitcpio -P\"");
        }
    }

    void unmount_target() {
        addLog("Unmounting target filesystems...", "#00ffff");
        execute_command("sudo umount -l /mnt 2>/dev/null || true");
    }

    void post_install_menu() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Installation Complete");
        dialog->setMinimumSize(400, 200);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        
        QLabel *completeLabel = new QLabel("╔══════════════════════════════════════════════╗\n"
                                           "║           INSTALLATION COMPLETE              ║\n"
                                           "╠══════════════════════════════════════════════╣\n"
                                           "║  1. Reboot now                              ║\n"
                                           "║  2. Exit to shell                           ║\n"
                                           "╚══════════════════════════════════════════════╝");
        completeLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(completeLabel);
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *rebootBtn = new QPushButton("🔄 Reboot Now");
        rebootBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; } QPushButton:hover { opacity: 0.8; }");
        rebootBtn->setMinimumHeight(45);
        QPushButton *shellBtn = new QPushButton("💻 Exit to Shell");
        shellBtn->setStyleSheet("QPushButton { background-color: #0088ff; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; } QPushButton:hover { opacity: 0.8; }");
        shellBtn->setMinimumHeight(45);
        
        btnLayout->addWidget(rebootBtn);
        btnLayout->addWidget(shellBtn);
        layout->addLayout(btnLayout);
        
        connect(rebootBtn, &QPushButton::clicked, [=]() {
            addLog("Unmounting and rebooting...", "#00ffff");
            unmount_target();
            execute_command("sudo reboot");
            dialog->accept();
        });
        
        connect(shellBtn, &QPushButton::clicked, [=]() {
            addLog("Exiting to shell. System is still mounted at /mnt", "#00ffff");
            dialog->accept();
        });
        
        dialog->exec();
        delete dialog;
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
        addLog("Setting timezone to: " + QString::fromStdString(timezone), "#00ffff");
        execute_command("sudo chroot /mnt /bin/bash -c \"ln -sf /usr/share/zoneinfo/" + timezone + " /etc/localtime\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"hwclock --systohc\"");
        addLog("Setting keyboard layout to: " + QString::fromStdString(keyboard_layout), "#00ffff");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo 'KEYMAP=" + keyboard_layout + "' > /etc/vconsole.conf\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\"");
        execute_command("sudo chroot /mnt /bin/bash -c \"locale-gen\"");
    }

    bool check_settings_configured() {
        QStringList errors;
        if (target_drive.empty()) errors << "Target drive not set! Use 'Setup Bootloader and Drive' first.";
        if (filesystem_type.empty()) errors << "Filesystem type not set! Use 'Setup Bootloader and Drive' first.";
        if (new_username.empty()) errors << "Username not set!";
        if (root_password.empty()) errors << "Root password not set!";
        if (user_password.empty()) errors << "User password not set!";
        if (timezone.empty()) errors << "Timezone not set!";
        if (keyboard_layout.empty()) errors << "Keyboard layout not set!";
        if (current_distro_name.empty()) errors << "No distribution selected!";
        
        if (!errors.isEmpty()) {
            QString errorMsg = "Configuration Errors:\n\n" + errors.join("\n");
            QMessageBox::critical(this, "Configuration Incomplete", errorMsg);
            for (const QString &err : errors) {
                addLog("Error: " + err, "#ff0000");
            }
            return false;
        }
        return true;
    }

    // GUI wrappers for all set_ functions
    void set_username() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Set Username");
        dialog->setMinimumSize(400, 200);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════╗\n║           Set Username                  ║\n╚══════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(titleLabel);
        
        QLabel *promptLabel = new QLabel("Enter username:");
        promptLabel->setStyleSheet("color: #00ffff; font-size: 14px; font-weight: bold;");
        layout->addWidget(promptLabel);
        
        QLineEdit *usernameInput = new QLineEdit();
        usernameInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; }");
        usernameInput->setText(QString::fromStdString(new_username));
        usernameInput->setMinimumHeight(35);
        layout->addWidget(usernameInput);
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("Set Username");
        okBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        btnLayout->addWidget(okBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        connect(okBtn, &QPushButton::clicked, [=]() {
            QString username = usernameInput->text().trimmed();
            if (!username.isEmpty()) {
                new_username = username.toStdString();
                saveConfiguration();
                display_current_settings();
                addLog("Username set to: " + username, "#00ff00");
                dialog->accept();
            }
        });
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void set_root_password() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Set Root Password");
        dialog->setMinimumSize(400, 250);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════╗\n║        Set Root Password               ║\n╚══════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(titleLabel);
        
        QLabel *promptLabel = new QLabel("Enter root password:");
        promptLabel->setStyleSheet("color: #00ffff; font-size: 14px; font-weight: bold;");
        layout->addWidget(promptLabel);
        
        QLineEdit *passwordInput = new QLineEdit();
        passwordInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; }");
        passwordInput->setEchoMode(QLineEdit::Password);
        passwordInput->setMinimumHeight(35);
        layout->addWidget(passwordInput);
        
        QLineEdit *confirmInput = new QLineEdit();
        confirmInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; }");
        confirmInput->setEchoMode(QLineEdit::Password);
        confirmInput->setPlaceholderText("Confirm root password");
        confirmInput->setMinimumHeight(35);
        layout->addWidget(confirmInput);
        
        QCheckBox *showPassword = new QCheckBox("Show password");
        showPassword->setStyleSheet("color: #aaaaaa;");
        connect(showPassword, &QCheckBox::toggled, [=](bool checked) {
            passwordInput->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
            confirmInput->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        });
        layout->addWidget(showPassword);
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("Set Password");
        okBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        btnLayout->addWidget(okBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        connect(okBtn, &QPushButton::clicked, [=]() {
            QString pass1 = passwordInput->text();
            QString pass2 = confirmInput->text();
            if (pass1.isEmpty()) {
                QMessageBox::warning(dialog, "Invalid Input", "Password cannot be empty!");
            } else if (pass1 != pass2) {
                QMessageBox::warning(dialog, "Password Mismatch", "Passwords do not match!");
            } else {
                root_password = pass1.toStdString();
                saveConfiguration();
                display_current_settings();
                addLog("Root password set successfully", "#00ff00");
                dialog->accept();
            }
        });
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void set_user_password() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Set User Password");
        dialog->setMinimumSize(400, 250);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════╗\n║        Set User Password               ║\n╚══════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(titleLabel);
        
        QLabel *promptLabel = new QLabel("Enter user password:");
        promptLabel->setStyleSheet("color: #00ffff; font-size: 14px; font-weight: bold;");
        layout->addWidget(promptLabel);
        
        QLineEdit *passwordInput = new QLineEdit();
        passwordInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; }");
        passwordInput->setEchoMode(QLineEdit::Password);
        passwordInput->setMinimumHeight(35);
        layout->addWidget(passwordInput);
        
        QLineEdit *confirmInput = new QLineEdit();
        confirmInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; }");
        confirmInput->setEchoMode(QLineEdit::Password);
        confirmInput->setPlaceholderText("Confirm user password");
        confirmInput->setMinimumHeight(35);
        layout->addWidget(confirmInput);
        
        QCheckBox *showPassword = new QCheckBox("Show password");
        showPassword->setStyleSheet("color: #aaaaaa;");
        connect(showPassword, &QCheckBox::toggled, [=](bool checked) {
            passwordInput->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
            confirmInput->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        });
        layout->addWidget(showPassword);
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("Set Password");
        okBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        btnLayout->addWidget(okBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        connect(okBtn, &QPushButton::clicked, [=]() {
            QString pass1 = passwordInput->text();
            QString pass2 = confirmInput->text();
            if (pass1.isEmpty()) {
                QMessageBox::warning(dialog, "Invalid Input", "Password cannot be empty!");
            } else if (pass1 != pass2) {
                QMessageBox::warning(dialog, "Password Mismatch", "Passwords do not match!");
            } else {
                user_password = pass1.toStdString();
                saveConfiguration();
                display_current_settings();
                addLog("User password set successfully", "#00ff00");
                dialog->accept();
            }
        });
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void set_timezone() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Select Timezone");
        dialog->setMinimumSize(500, 500);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════╗\n║         Select Timezone                ║\n╚══════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(titleLabel);
        
        QListWidget *timezoneList = new QListWidget();
        timezoneList->setStyleSheet("QListWidget { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; font-size: 14px; } QListWidget::item { padding: 10px; } QListWidget::item:selected { background-color: #444; }");
        
        QStringList timezones = {"America/New_York (US English)", "Europe/London (UK English)", "Europe/Berlin (German)", "Europe/Paris (French)", "Europe/Madrid (Spanish)", "Europe/Rome (Italian)", "Asia/Tokyo (Japanese)", "Other (manual entry)"};
        QStringList tzValues = {"America/New_York", "Europe/London", "Europe/Berlin", "Europe/Paris", "Europe/Madrid", "Europe/Rome", "Asia/Tokyo", ""};
        
        timezoneList->addItems(timezones);
        layout->addWidget(timezoneList);
        
        QWidget *manualWidget = new QWidget();
        QHBoxLayout *manualLayout = new QHBoxLayout(manualWidget);
        QLabel *manualLabel = new QLabel("Custom timezone:");
        manualLabel->setStyleSheet("color: #00ffff;");
        QLineEdit *manualInput = new QLineEdit();
        manualInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; }");
        manualInput->setPlaceholderText("e.g., Europe/Berlin");
        manualLayout->addWidget(manualLabel);
        manualLayout->addWidget(manualInput);
        manualWidget->setVisible(false);
        layout->addWidget(manualWidget);
        
        connect(timezoneList, &QListWidget::currentRowChanged, [=](int row) {
            manualWidget->setVisible(row == 7);
        });
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("Set Timezone");
        okBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        btnLayout->addWidget(okBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        connect(okBtn, &QPushButton::clicked, [=]() {
            int idx = timezoneList->currentRow();
            if (idx >= 0) {
                if (idx == 7) {
                    QString customTz = manualInput->text().trimmed();
                    if (!customTz.isEmpty()) timezone = customTz.toStdString();
                } else {
                    timezone = tzValues[idx].toStdString();
                }
                saveConfiguration();
                display_current_settings();
                addLog("Timezone set to: " + QString::fromStdString(timezone), "#00ff00");
                dialog->accept();
            }
        });
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void set_keyboard_layout() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Select Keyboard Layout");
        dialog->setMinimumSize(500, 500);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════╗\n║      Select Keyboard Layout            ║\n╚══════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(titleLabel);
        
        QListWidget *layoutList = new QListWidget();
        layoutList->setStyleSheet("QListWidget { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; font-size: 14px; } QListWidget::item { padding: 10px; } QListWidget::item:selected { background-color: #444; }");
        
        QStringList layouts = {"us (US English)", "uk (UK English)", "de (German)", "fr (French)", "es (Spanish)", "it (Italian)", "jp (Japanese)", "Other (manual entry)"};
        QStringList layoutValues = {"us", "uk", "de", "fr", "es", "it", "jp", ""};
        
        layoutList->addItems(layouts);
        layout->addWidget(layoutList);
        
        QWidget *manualWidget = new QWidget();
        QHBoxLayout *manualLayout = new QHBoxLayout(manualWidget);
        QLabel *manualLabel = new QLabel("Custom layout:");
        manualLabel->setStyleSheet("color: #00ffff;");
        QLineEdit *manualInput = new QLineEdit();
        manualInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; }");
        manualInput->setPlaceholderText("e.g., br, ru, pt");
        manualLayout->addWidget(manualLabel);
        manualLayout->addWidget(manualInput);
        manualWidget->setVisible(false);
        layout->addWidget(manualWidget);
        
        connect(layoutList, &QListWidget::currentRowChanged, [=](int row) {
            manualWidget->setVisible(row == 7);
        });
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("Set Layout");
        okBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        btnLayout->addWidget(okBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        connect(okBtn, &QPushButton::clicked, [=]() {
            int idx = layoutList->currentRow();
            if (idx >= 0) {
                if (idx == 7) {
                    QString customLayout = manualInput->text().trimmed();
                    if (!customLayout.isEmpty()) keyboard_layout = customLayout.toStdString();
                } else {
                    keyboard_layout = layoutValues[idx].toStdString();
                }
                saveConfiguration();
                display_current_settings();
                addLog("Keyboard layout set to: " + QString::fromStdString(keyboard_layout), "#00ff00");
                dialog->accept();
            }
        });
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void set_wireless_regdom() {
        std::string currentDir = getCurrentDir();
        std::string wireless_regdom_file = currentDir + "/needed-files/wireless-regdom";
        addLog("Opening wireless regulatory domain file for editing...", "#00ffff");
        addLog("File: " + QString::fromStdString(wireless_regdom_file), "#ffff00");
        addLog("Use Ctrl+X to exit nano after editing", "#ffff00");
        
        QMessageBox::information(this, "Wireless Regdom", 
            QString("Edit the file:\n%1\n\nUse an external editor to modify this file.").arg(QString::fromStdString(wireless_regdom_file)));
        
        execute_command("nano " + wireless_regdom_file);
        addLog("Wireless regulatory domain file updated.", "#00ff00");
    }

    void set_extra_packages() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Install Extra Packages");
        dialog->setMinimumSize(500, 300);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════╗\n║      Install Extra Packages            ║\n╚══════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(titleLabel);
        
        QLabel *promptLabel = new QLabel("Enter extra packages (space separated):");
        promptLabel->setStyleSheet("color: #00ffff; font-size: 14px; font-weight: bold;");
        layout->addWidget(promptLabel);
        
        QLineEdit *packagesInput = new QLineEdit();
        packagesInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; }");
        packagesInput->setText(QString::fromStdString(extra_packages));
        packagesInput->setMinimumHeight(35);
        layout->addWidget(packagesInput);
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *okBtn = new QPushButton("Set Packages");
        okBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 10px 20px; border: none; border-radius: 5px; }");
        btnLayout->addWidget(okBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        connect(okBtn, &QPushButton::clicked, [=]() {
            extra_packages = packagesInput->text().trimmed().toStdString();
            saveConfiguration();
            display_current_settings();
            if (extra_packages.empty()) {
                addLog("Extra packages cleared.", "#ffff00");
            } else {
                addLog("Extra packages set to: " + QString::fromStdString(extra_packages), "#00ff00");
            }
            dialog->accept();
        });
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void show_distro_selection() {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Select Distribution to Install");
        dialog->setMinimumSize(600, 600);
        dialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *titleLabel = new QLabel("╔══════════════════════════════════════════╗\n║   Select Distribution to Install       ║\n╚══════════════════════════════════════════╝");
        titleLabel->setStyleSheet("color: #00ffff; font-family: 'Courier New', monospace; font-size: 12px;");
        layout->addWidget(titleLabel);
        
        QTabWidget *tabWidget = new QTabWidget();
        tabWidget->setStyleSheet("QTabWidget::pane { background-color: #2d2d2d; border: 1px solid #555; } QTabBar::tab { background-color: #333; color: #00ffff; padding: 10px 20px; font-weight: bold; } QTabBar::tab:selected { background-color: #444; border-bottom: 3px solid #ff6600; }");
        
        QWidget *spitfireTab = new QWidget();
        QVBoxLayout *spitfireLayout = new QVBoxLayout(spitfireTab);
        QListWidget *spitfireList = new QListWidget();
        spitfireList->setStyleSheet("QListWidget { background-color: #2d2d2d; color: #ff6600; border: none; font-size: 14px; } QListWidget::item { padding: 12px; border-bottom: 1px solid #444; } QListWidget::item:selected { background-color: #444; color: #ffff00; }");
        spitfireList->addItems({"Install Spitfire CKGE Minimal", "Install Spitfire CKGE Minimal Dev", "Install Spitfire CKGE Full", "Install Spitfire CKGE Full Dev", "Install Spitfire CKGE Black Full", "Install Spitfire CKGE Black Full Dev"});
        spitfireLayout->addWidget(spitfireList);
        tabWidget->addTab(spitfireTab, "Spitfire Series");
        
        QWidget *apexTab = new QWidget();
        QVBoxLayout *apexLayout = new QVBoxLayout(apexTab);
        QListWidget *apexList = new QListWidget();
        apexList->setStyleSheet("QListWidget { background-color: #2d2d2d; color: #aa00aa; border: none; font-size: 14px; } QListWidget::item { padding: 12px; border-bottom: 1px solid #444; } QListWidget::item:selected { background-color: #444; color: #ffff00; }");
        apexList->addItems({"Install Apex CKGE Minimal", "Install Apex CKGE Minimal Dev", "Install Apex CKGE Full", "Install Apex CKGE Full Dev"});
        apexLayout->addWidget(apexList);
        tabWidget->addTab(apexTab, "Apex Series");
        
        layout->addWidget(tabWidget);
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *selectBtn = new QPushButton("Select Distribution");
        selectBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; }");
        selectBtn->setMinimumHeight(45);
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #cc0000; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; }");
        cancelBtn->setMinimumHeight(45);
        QPushButton *backBtn = new QPushButton("Back to Main Menu");
        backBtn->setStyleSheet("QPushButton { background-color: #555; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; }");
        backBtn->setMinimumHeight(45);
        btnLayout->addWidget(selectBtn);
        btnLayout->addWidget(backBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        QStringList spitfireValues = {"Spitfire-CKGE-Minimal", "Spitfire-CKGE-Minimal-Dev", "Spitfire-CKGE-Full", "Spitfire-CKGE-Full-Dev", "Spitfire-CKGE-Black-Full", "Spitfire-CKGE-Black-Full-Dev"};
        QStringList apexValues = {"Apex-CKGE-Minimal", "Apex-CKGE-Minimal-Dev", "Apex-CKGE-Full", "Apex-CKGE-Full-Dev"};
        
        connect(selectBtn, &QPushButton::clicked, [=]() {
            int tabIdx = tabWidget->currentIndex();
            QString selectedDistro;
            
            if (tabIdx == 0 && spitfireList->currentRow() >= 0) {
                selectedDistro = spitfireValues[spitfireList->currentRow()];
            } else if (tabIdx == 1 && apexList->currentRow() >= 0) {
                selectedDistro = apexValues[apexList->currentRow()];
            }
            
            if (!selectedDistro.isEmpty()) {
                current_distro_name = selectedDistro.toStdString();
                saveConfiguration();
                display_current_settings();
                addLog(selectedDistro + " selected. Use 'Start Installation' to begin.", "#00ff00");
                dialog->accept();
            }
        });
        
        connect(backBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        connect(cancelBtn, &QPushButton::clicked, [=]() { dialog->reject(); });
        
        dialog->exec();
        delete dialog;
    }

    void start_installation() {
        if (!check_settings_configured()) {
            return;
        }
        
        // Confirmation dialog
        QDialog *confirmDialog = new QDialog(this);
        confirmDialog->setWindowTitle("⚠️ Start Installation - Confirmation Required");
        confirmDialog->setMinimumSize(600, 400);
        confirmDialog->setStyleSheet("QDialog { background-color: #1e1e1e; }");
        
        QVBoxLayout *layout = new QVBoxLayout(confirmDialog);
        
        QLabel *warningLabel = new QLabel("⚠️  WARNING: DESTRUCTIVE OPERATION");
        warningLabel->setStyleSheet("color: #ff0000; font-size: 18px; font-weight: bold; text-align: center; background-color: #330000; padding: 15px; border: 2px solid #ff0000; border-radius: 5px;");
        layout->addWidget(warningLabel);
        
        QLabel *summaryLabel = new QLabel();
        summaryLabel->setStyleSheet("color: #ffff00; font-size: 14px; font-family: 'Courier New', monospace; background-color: #2d2d2d; padding: 15px; border-radius: 5px;");
        summaryLabel->setText(QString(
            "═══════════════════════════════════════\n"
            "  Installation Summary:\n"
            "  ─────────────────────────────────\n"
            "  Drive:      %1\n"
            "  Filesystem: %2\n"
            "  Distro:     %3\n"
            "  Username:   %4\n"
            "  Timezone:   %5\n"
            "  Keyboard:   %6\n"
            "  ─────────────────────────────────\n"
            "  ALL DATA ON %1 WILL BE DESTROYED!\n"
            "═══════════════════════════════════════"
        ).arg(QString::fromStdString(target_drive), QString::fromStdString(filesystem_type), QString::fromStdString(current_distro_name), QString::fromStdString(new_username), QString::fromStdString(timezone), QString::fromStdString(keyboard_layout)));
        layout->addWidget(summaryLabel);
        
        QLabel *confirmText = new QLabel("Type 'YES' to confirm installation:");
        confirmText->setStyleSheet("color: #ff0000; font-size: 14px; font-weight: bold;");
        layout->addWidget(confirmText);
        
        QLineEdit *confirmInput = new QLineEdit();
        confirmInput->setStyleSheet("QLineEdit { background-color: #2d2d2d; color: #00ff00; border: 1px solid #555; padding: 8px; border-radius: 5px; font-size: 13px; }");
        confirmInput->setPlaceholderText("Type YES to continue");
        layout->addWidget(confirmInput);
        
        QProgressBar *progressBar = new QProgressBar();
        progressBar->setStyleSheet("QProgressBar { border: 2px solid #555; border-radius: 5px; text-align: center; height: 25px; } QProgressBar::chunk { background-color: #ff6600; border-radius: 3px; }");
        progressBar->setVisible(false);
        layout->addWidget(progressBar);
        
        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *startBtn = new QPushButton("🚀 Start Installation");
        startBtn->setStyleSheet("QPushButton { background-color: #ff0000; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; }");
        startBtn->setMinimumHeight(45);
        QPushButton *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet("QPushButton { background-color: #00aa00; color: white; font-weight: bold; padding: 12px 25px; border: none; border-radius: 5px; font-size: 14px; }");
        cancelBtn->setMinimumHeight(45);
        btnLayout->addWidget(startBtn);
        btnLayout->addWidget(cancelBtn);
        layout->addLayout(btnLayout);
        
        connect(startBtn, &QPushButton::clicked, [=]() {
            if (confirmInput->text() == "YES") {
                startBtn->setEnabled(false);
                progressBar->setVisible(true);
                progressBar->setMaximum(0);
                
                addLog("\n═══════════════════════════════════════", "#ff6600");
                addLog("  STARTING INSTALLATION", "#ff6600");
                addLog("═══════════════════════════════════════\n", "#ff6600");
                
                QTimer::singleShot(500, this, [=]() {
                    confirmDialog->accept();
                    m_busy = true;
                    progressBar->setMaximum(100);
                    progressBar->setValue(5);
                    
                    // Prepare partitions and mount filesystem directly
                    prepare_target_partitions();
                    progressBar->setValue(15);
                    
                    if (filesystem_type == "btrfs") {
                        setup_btrfs_subvolumes();
                    } else {
                        setup_ext4_filesystem();
                    }
                    progressBar->setValue(25);
                    
                    // Call the appropriate install function
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
                        addLog("No valid distribution selected!", "#ff0000");
                    }
                    
                    progressBar->setValue(100);
                    m_busy = false;
                });
            } else {
                QMessageBox::warning(confirmDialog, "Confirmation Required", "Please type 'YES' to confirm installation.");
            }
        });
        
        connect(cancelBtn, &QPushButton::clicked, [=]() { confirmDialog->reject(); });
        
        confirmDialog->exec();
        delete confirmDialog;
    }

    // ========================================================================
    // ALL ORIGINAL INSTALL FUNCTIONS - COMPLETELY PRESERVED
    // ========================================================================

    void install_spitfire_ckge_minimal() {
        addLog("Installing Spitfire CKGE Minimal to " + QString::fromStdString(target_drive) + "...", "#ff6600");
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        addLog("Installing base system with pacstrap...", "#00ffff");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_spitfire_ckge_minimal_dev() {
        addLog("Installing Spitfire CKGE Minimal Dev to " + QString::fromStdString(target_drive) + "...", "#ff6600");
        std::string target_folder = "/mnt";
        std::string currentDir = getCurrentDir();
        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);
        addLog("Installing base system with pacstrap...", "#00ffff");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_spitfire_ckge_full() {
        addLog("Installing Spitfire CKGE Full to " + QString::fromStdString(target_drive) + "...", "#ff6600");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_spitfire_ckge_full_dev() {
        addLog("Installing Spitfire CKGE Full Dev to " + QString::fromStdString(target_drive) + "...", "#ff6600");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_spitfire_ckge_black_full() {
        addLog("Installing Spitfire CKGE Black Full to " + QString::fromStdString(target_drive) + "...", "#ff6600");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_spitfire_ckge_black_full_dev() {
        addLog("Installing Spitfire CKGE Black Full Dev to " + QString::fromStdString(target_drive) + "...", "#ff6600");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_apex_ckge_minimal() {
        addLog("Installing Apex CKGE Minimal to " + QString::fromStdString(target_drive) + "...", "#aa00aa");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_apex_ckge_minimal_dev() {
        addLog("Installing Apex CKGE Minimal Dev to " + QString::fromStdString(target_drive) + "...", "#aa00aa");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_apex_ckge_full() {
        addLog("Installing Apex CKGE Full to " + QString::fromStdString(target_drive) + "...", "#aa00aa");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }

    void install_apex_ckge_full_dev() {
        addLog("Installing Apex CKGE Full Dev to " + QString::fromStdString(target_drive) + "...", "#aa00aa");
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
        addLog("\nInstallation complete! System installed to " + QString::fromStdString(target_drive), "#00ff00");
        post_install_menu();
    }
};

// Main function
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("claudemods distribution iso creator");
    app.setApplicationVersion("1.01");
    app.setOrganizationName("claudemods");
    app.setStyle("Fusion");
    
    app.setStyleSheet(
        "QToolTip { color: #00ffff; background-color: #222; border: 1px solid #555; padding: 5px; }"
        "QMessageBox { background-color: #1e1e1e; color: #ffffff; }"
        "QMessageBox QLabel { color: #ffffff; }"
        "QMessageBox QPushButton { min-width: 80px; min-height: 30px; }"
    );
    
    ClaudemodsInstallerQt installer;
    installer.show();
    
    return app.exec();
}

#include "claudemods.moc"
