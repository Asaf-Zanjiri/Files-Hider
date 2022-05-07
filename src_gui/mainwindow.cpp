#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this); // Load the UI form.

    // Try to stop and delete the service in-case it's already running to avoid driver start conflicts/errors.
    stopService((char *)SERVICE_NAME);
    deleteService((char *)SERVICE_NAME);

    setWindowTitle(tr("Files Hider | Asaf Zanjiri")); // GUI Title
    statusBar()->showMessage(tr("Status: Welcome!")); // Set a status message at the bottom of the GUI.

    // Initializing a context menu for the tray app.
    QMenu *menu = new QMenu(this);
    QAction *minimizeWindow = new QAction(tr("Minimize to Tray"), this);
    QAction *showWindow = new QAction(tr("Show Interface"), this);
    QAction *exitAction = new QAction(tr("Exit"), this);

    // Connect the signals clicks on menu items to by appropriate slots. The first menu item expands the application from the tray. The second menu item terminates the application.
    connect(minimizeWindow, SIGNAL(triggered()), this, SLOT(hide())); // Minimizes the user interface when clicked.
    connect(showWindow, SIGNAL(triggered()), this, SLOT(show())); // Shows the user interface when clicked.
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close())); // Closes the application when exit is clicked.
    menu->addAction(minimizeWindow);
    menu->addAction(showWindow);
    menu->addAction(exitAction);

    // Initialize the tray application.
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon)); // Set a default computer icon for the tray app.
    trayIcon->setToolTip(tr("Files Hider")); // Tray app name.
    trayIcon->setContextMenu(menu); // Add the context menu to the tray app.
    trayIcon->show(); // Show the tray icon with all the other tray icons.
}

MainWindow::~MainWindow()
{
    // Try to stop and delete the service in-case it's already running to avoid driver start conflicts/errors in the next run.
    stopService((char *)SERVICE_NAME);
    deleteService((char *)SERVICE_NAME);

    delete ui;
}

bool MainWindow::SanitizePath(wchar_t *path)
{
    // Enforce the path doesn't end with a backslash to avoid path errors.
    if(path[wcslen(path) - 1] == L'\\')
        path[wcslen(path) - 1] = L'\0';

    // Check if the text inside `inputPath` is a valid file path.
    DWORD result = GetFileAttributesW(path);
    if(result==INVALID_FILE_ATTRIBUTES)
    {
        QMessageBox::warning(this, tr("Error Removing"), tr("Given file path was invalid. Please change it to a valid file path."));
        return false;
    }

    return true;
}

/* Add/Remove files to hide */

void MainWindow::on_btnBrowseFiles_clicked()
{
    // add if accoridng to checkbox
    QString filePath;

    // Open file select dialog and let the user choose a path to hide.
    if(ui->checkBoxToggleSelectFolders->isChecked())
        filePath = QFileDialog::getExistingDirectory(this, tr("Select a Directory to Hide"));
    else
        filePath = QFileDialog::getOpenFileName(this, tr("Select a File to Hide"));

    if(filePath != "")
        ui->inputPath->setText(filePath); // Set inputPath's text as the path of the selected file.
}

void MainWindow::on_btnAddFile_clicked()
{
    // Check if the user tries to go over the hide list limit.
    if(ui->listPaths->count() >= ARRAY_SIZE)
    {
        QMessageBox::warning(this, tr("Error"), QString("Can't add path. Hide list is capped at %1 items. You can manually hide files by adding the %2 prefix to their name").arg(ARRAY_SIZE).arg(HIDE_PREFIX));
        return;
    }

    // Initialize Variables.
    wchar_t path[MAX_PATH];
    wcscpy_s(path, ui->inputPath->text().replace("/", "\\").toStdWString().c_str());

    // Sanitize the file path.
    if(!SanitizePath(path))
        return;

    // Check if path already exists in the list.
    for(int i = 0; i< ui->listPaths->count(); ++i)
    {
        // Try to send the path of the item to the hide driver to add it to its internal hide list. If fails, delete the item and correct the number of items in the list.
        QListWidgetItem* item = ui->listPaths->item(i);
        if(wcscmp(path, item->text().toStdWString().c_str()) == 0)
        {
            QMessageBox::warning(this, tr("Error"), tr("Given file path was is already in the hide list."));
            return;
        }
    }

    // If the driver is enabled. Try to send the path to the hide driver to add to its internal hide list. Else it'll be automatically added when the user starts the driver.
    if(g_status)
        if(!sendPathToDriver(path, true))
            return;

    // Add path to List.
    ui->listPaths->addItem(QString::fromWCharArray(path));

}

void MainWindow::on_btnRemoveFile_clicked()
{
    for(QListWidgetItem *item : ui->listPaths->selectedItems())
    {
        // Initialize Variables.
        wchar_t path[MAX_PATH];
        wcscpy_s(path, item->text().replace("/", "\\").toStdWString().c_str());

        // Sanitize the file path.
        if(!SanitizePath(path))
            continue;

        // If the driver is enabled. Try to send the path of the item to the hide driver to remove from its internal hide list. Else it'll be automatically removed when the user starts the driver.
        if(g_status)
            if(!sendPathToDriver(path, false))
                continue;

        // Remove Path from list.
        delete item;
    }
}

bool MainWindow::sendPathToDriver(wchar_t* path, bool sendAdd)
{
    // Initialize variables.
    HANDLE hFileHandle;
    wchar_t devicePath[MAX_PATH];
    wchar_t hidePath[MAX_PATH + wcslen(ADD_CMD_PREFIX) + wcslen(REMOVE_CMD_PREFIX)]; // Added more to cover the command at the begining (e.g. `remove.`)
    bool resultBool;

    // Turn the path name to a path the driver can interpret (e.g. `C:` -> `\Device\HarddiskVolume2`) then concatenates to the full path.
    QueryDosDeviceW(QString::fromWCharArray(path, 2).toStdWString().c_str(), devicePath, MAX_PATH);
    wcscat_s(devicePath, &path[2]);

    // Open handle to Driver.
    hFileHandle = device_io::GetHandleToIo((char*)DEVICE_NAME);
    if(hFileHandle == INVALID_HANDLE_VALUE)
    {
        statusBar()->showMessage(tr("Status: Error creating handle to driver.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Write to Driver.
    if(sendAdd)
        wcscpy_s(hidePath, wcslen(ADD_CMD_PREFIX) + 1, ADD_CMD_PREFIX);
    else
        wcscpy_s(hidePath, wcslen(REMOVE_CMD_PREFIX) + 1, REMOVE_CMD_PREFIX);
    wcscat_s(hidePath, MAX_PATH, devicePath);
    device_io::WriteToDevice(hFileHandle, hidePath);

    // Close handle to Driver.
    resultBool = device_io::CloseIoHandle(hFileHandle);
    if(resultBool == false)
    {
        statusBar()->showMessage(tr("Status: Failed to close handle to driver.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    return true;
}

void MainWindow::on_listPaths_itemDoubleClicked(QListWidgetItem *item)
{
    ui->inputPath->setText(item->text()); // Set inputPath's text as the path of the selected file.
}


/* Create/Start, Delete/Stop Driver */

void MainWindow::on_btnStart_clicked()
{
    // Declaring Variables
    bool result;

    // Setup Service
    result = createService((char *)SERVICE_NAME, (char *)DRIVER_NAME, true);
    if(result == false)
        return;

    // Start Service
    result = startService((char *)SERVICE_NAME);
    if(result == false)
    {
        deleteService((char *)SERVICE_NAME); // Cleanup
        statusBar()->showMessage(tr("Status: Failed to start driver."));
        return;
    }

    // Disables start button and enables stop button.
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);


    // Iterate over the paths list and send any pre-existing paths to the driver.
    for(int i = 0; i< ui->listPaths->count(); ++i)
    {
        // Try to send the path of the item to the hide driver to add it to its internal hide list. If fails, delete the item and correct the number of items in the list.
        QListWidgetItem* item = ui->listPaths->item(i);
        if(!sendPathToDriver((wchar_t*)item->text().toStdWString().c_str(), true))
        {
            delete item;
            i--;
        }
    }


    g_status = true;
}

void MainWindow::on_btnStop_clicked()
{
    // Declaring Variables
    bool result;

    // Stop Service
    result = stopService((char *)SERVICE_NAME);
    if(result == false)
        return;

    // Delete Service
    result = deleteService((char *)SERVICE_NAME);
    if(result == false)
        return;

    // Disables stop button and enables start button.
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);

    g_status = false;
}


/* Service Tools */

bool MainWindow::createService(char* serviceName, char* executablePath, bool relative)
{
    // Initializing Variables
    char pathBuf[MAX_PATH];
    statusBar()->showMessage(tr("Status: Creating service ") + serviceName); // Set a status message at the bottom of the GUI.

    // If the path is relative, add the current directory before the executable path.
    if(relative)
    {
        GetCurrentDirectoryA(MAX_PATH, pathBuf);
        strcat_s(pathBuf, "\\");
    }
    strcat_s(pathBuf, executablePath);

    // Try to open a handle to the service manager.
    SC_HANDLE sh = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if(sh == INVALID_HANDLE_VALUE)
    {
        statusBar()->showMessage(tr("Status: Error opening handle to Service Manager.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Create Service
    SC_HANDLE hService = CreateServiceA(sh, serviceName, serviceName, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, pathBuf, 0, 0, 0, 0, 0);
    CloseServiceHandle(sh);
    if(hService == 0)
    {
        statusBar()->showMessage(tr("Status: Error creating service.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Success Creating Service
    statusBar()->showMessage(tr("Status: Service has been created under the display name of ") + serviceName); // Set a status message at the bottom of the GUI.
    CloseServiceHandle(hService);

    return true;
}

bool MainWindow::startService(char* serviceName)
{
    // Initialize variables
    SC_HANDLE hService;
    SC_HANDLE sh = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);

    // Try opening a service handler handle.
    if(!sh)
    {
        statusBar()->showMessage(tr("Status: Error opening service handler handle.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Try opening a handle to service manager.
    hService = OpenServiceA(sh, serviceName, SERVICE_ALL_ACCESS);
    CloseServiceHandle(sh);
    if(!hService)
    {
        statusBar()->showMessage(tr("Status: Error opening service handle.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Start the service.
    if(StartService(hService, 0, 0) != 0)
        statusBar()->showMessage(tr("Status: Service started.")); // Set a status message at the bottom of the GUI.
    else
    {
        unsigned long eCode = GetLastError();
        statusBar()->showMessage(tr("Status: Error starting service. Err Code ") + QString::number(eCode)); // Set a status message at the bottom of the GUI.
        CloseServiceHandle(hService);
        return false;
    }
    CloseServiceHandle(hService);

    return true;
}

bool MainWindow::stopService(char* serviceName)
{
    // Initialize Variables
    SC_HANDLE hService;
    SC_HANDLE sh = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);

    // Try opening a service handler handle.
    if(!sh)
    {
        statusBar()->showMessage(tr("Status: Error opening service handler handle.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Try opening a handle to service manager.
    hService = OpenServiceA(sh, serviceName, SERVICE_ALL_ACCESS);
    CloseServiceHandle(sh);
    if(!hService)
    {
        statusBar()->showMessage(tr("Status: Error opening service handle.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Stop the service.
    SERVICE_STATUS srvStatus;
    if(ControlService(hService, SERVICE_CONTROL_STOP, &srvStatus) != 0)
        statusBar()->showMessage(tr("Status: Service stopped.")); // Set a status message at the bottom of the GUI.
    else
    {
        statusBar()->showMessage(tr("Status: Error stopping service.")); // Set a status message at the bottom of the GUI.
        CloseServiceHandle(hService);
        return false;
    }

    CloseServiceHandle(hService);
    return true;
}

bool MainWindow::deleteService(char* serviceName)
{
    // Initialize Variables
    SC_HANDLE hService;
    SC_HANDLE sh = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);

    // Try opening a service handler handle.
    if(!sh)
    {
        statusBar()->showMessage(tr("Status: Error opening service handler handle.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Try opening a handle to service manager.
    hService = OpenServiceA(sh, serviceName, SERVICE_ALL_ACCESS);
    CloseServiceHandle(sh);
    if(!hService)
    {
        statusBar()->showMessage(tr("Status: Error opening service handle.")); // Set a status message at the bottom of the GUI.
        return false;
    }

    // Delete the service
    if(DeleteService(hService) != 0)
        statusBar()->showMessage(tr("Status: Service deleted.")); // Set a status message at the bottom of the GUI.
    else
    {
        statusBar()->showMessage(tr("Status: Error deleting service.")); // Set a status message at the bottom of the GUI.
        CloseServiceHandle(hService);
        return false;
    }
    CloseServiceHandle(hService);

    return true;
}

/* Options checkboxes */

void MainWindow::on_checkBoxEnableHidePrefix_stateChanged(int checkState)
{
    // Open handle to Driver.
    HANDLE hFileHandle = device_io::GetHandleToIo((char*)DEVICE_NAME);
    if(hFileHandle == INVALID_HANDLE_VALUE)
    {
        statusBar()->showMessage(tr("Status: Error creating handle to driver.")); // Set a status message at the bottom of the GUI.
        return;
    }

    // Write to Driver.
    if(checkState == Qt::Checked)
        device_io::WriteToDevice(hFileHandle, (wchar_t *)ENABLE_HIDE_PREFIX);
    else
        device_io::WriteToDevice(hFileHandle, (wchar_t *)DISABLE_HIDE_PREFIX);

    // Close handle to Driver.
    bool result = device_io::CloseIoHandle(hFileHandle);
    if(result == false)
        statusBar()->showMessage(tr("Status: Failed to close handle to driver.")); // Set a status message at the bottom of the GUI.
}

