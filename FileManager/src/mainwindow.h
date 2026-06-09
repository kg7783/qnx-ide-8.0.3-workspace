#pragma once

#include <QMainWindow>
#include <QStack>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>

class QTreeView;
class QListView;
class QTableView;
class QLineEdit;
class QLabel;
class QStackedWidget;
class QAction;
#include <QComboBox>

class FileFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void navigateTo(const QModelIndex &index);
    void navigateToPath(const QString &path);
    void treeClick(const QModelIndex &index);
    void listDoubleClick(const QModelIndex &index);
    void selectionChanged();
    void goUp();
    void goBack();
    void goForward();
    void goHome();
    void goRoot();
    void refresh();
    void addressActivated();
    void newFolder();
    void newFile();
    void deleteFiles();
    void renameFile();
    void showProperties();
    void copyFiles();
    void cutFiles();
    void pasteFiles();
    void setViewMode(int mode);
    void toggleHidden();
    void showTreeContextMenu(const QPoint &pos);
    void showListContextMenu(const QPoint &pos);

private:
    void setupUI();
    void connectSelectionSignals();
    void setupToolbar();
    void setupMenus();
    void setupStatusBar();
    void setupModels();
    void updateNavButtons();
    void updateAddressBar();
    void updateStatusBar();
    QString currentPath() const;
    QStringList selectedFiles() const;
    void copyToClipboard(const QStringList &files, bool cut);
    void pasteFromClipboard();
    void openFile(const QModelIndex &index);
    void openEditor(const QString &path);
    QMenu * buildContextMenu(const QPoint &globalPos, bool isDir);

    QTreeView *m_tree;
    QStackedWidget *m_viewStack;
    QListView *m_listView;
    QTableView *m_tableView;
    QLineEdit *m_addressBar;
    QLabel *m_statusLabel;

    QFileSystemModel *m_model;
    FileFilterProxy *m_proxy;

    QStack<QString> m_backStack;
    QStack<QString> m_forwardStack;

    QStringList m_clipboardFiles;
    bool m_clipboardCut = false;

    QAction *m_actBack;
    QAction *m_actForward;
    QAction *m_actUp;
    QAction *m_actHome;
    QAction *m_actRefresh;
    QAction *m_actNewFolder;
    QAction *m_actNewFile;
    QAction *m_actDelete;
    QAction *m_actRename;
    QAction *m_actCopy;
    QAction *m_actCut;
    QAction *m_actPaste;
    QAction *m_actProperties;
    QAction *m_actHidden;

    int m_viewMode = 0;
};
