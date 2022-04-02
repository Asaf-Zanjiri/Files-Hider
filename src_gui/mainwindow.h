#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

#include <windows.h>

#include "device_io.h"

// These names are based on the names set in the driver. If you want to change them make sure you also change them in the driver source as well.
#define SERVICE_NAME "FILE_HIDER_DRIVER"
#define DEVICE_NAME "\\\\.\\FilesHider"
#define DRIVER_NAME "FilesHider.sys"
#define ADD_CMD_PREFIX L"add." // Add path to list command operation prefix
#define REMOVE_CMD_PREFIX L"del." // Remove path from list command operation prefix

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /// Called upon button click. Opens file select dialog, Sets the inputPath textbox to a path of the selected file.
    void on_btnBrowseFiles_clicked();

    /// Called upon button click. Checks if the content of inputPath a valid path. If it is, it sends it to the driver.
    void on_btnAddFile_clicked();

    /// Called upon button click. Create and start hiding driver.
    void on_btnStart_clicked();

    /// Called upon button click. Delete and stop hiding driver.
    void on_btnStop_clicked();

    /// Called upon button click. Removes selected path from list and send the driver a remove command.
    void on_btnRemoveFile_clicked();

    /// Called upon double click on a path from the list. Sets the input textbox text to the path double clicked on in the list.
    void on_listPaths_itemDoubleClicked(QListWidgetItem *item);


private:
    Ui::MainWindow *ui;
    bool g_status = false; // Status of if the driver is enabled or disabled.
    QSystemTrayIcon *trayIcon; // Declare the object of future applications for the tray icon

    /// Tries to create a new windows service. Returns true if the operation succeeded.
    bool createService(char* serviceName, char* executablePath, bool relative);

    /// Tries to start a windows service. Returns true if the operation succeeded.
    bool startService(char* serviceName);

    /// Tries to stop a windows service. Returns true if the operation succeeded.
    bool stopService(char* serviceName);

    /// Tries to delete a new windows service. Returns true if the operation succeeded.
    bool deleteService(char* serviceName);

    /// Sends a path to the driver. If sendAdd is set to True, then it'll send a add command to the driver, otherwise it'll send a remove command.
    bool sendPathToDriver(wchar_t* path, bool sendAdd);

};
#endif // MAINWINDOW_H
