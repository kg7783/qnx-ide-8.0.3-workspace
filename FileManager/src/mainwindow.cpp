#include "mainwindow.h"
#include "editor.h"

#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QDir>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QKeyEvent>
#include <QCompleter>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QPainter>
#include <QFileIconProvider>
#include <QStyle>
#include <QPoint>

// ── Proxy: filter out dot-files unless hidden shown ──

bool FileFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    QFileSystemModel *fsm = qobject_cast<QFileSystemModel*>(sourceModel());
    if (!fsm) return true;

    QModelIndex idx = fsm->index(row, 0, parent);
    QString name = fsm->fileName(idx);

    if (name.startsWith('.'))
        return filterRegularExpression().pattern().isEmpty();

    return true;
}

// ── MainWindow ──

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("QNX File Manager");
    resize(950, 620);
    setupModels();
    setupUI();
    setupToolbar();
    setupMenus();
    setupStatusBar();

    navigateToPath("/");
    connectSelectionSignals();
}

// ── Models ──

void MainWindow::setupModels()
{
    m_model = new QFileSystemModel(this);
    m_model->setRootPath("/");
    m_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    m_proxy = new FileFilterProxy(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterRegularExpression(QRegularExpression());
}

// ── UI ──

void MainWindow::setupUI()
{
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    // --- Tree ---
    m_tree = new QTreeView;
    m_tree->setModel(m_proxy);
    m_tree->hideColumn(1);
    m_tree->hideColumn(2);
    m_tree->hideColumn(3);
    m_tree->setHeaderHidden(true);
    m_tree->setAnimated(true);
    m_tree->setIndentation(16);
    m_tree->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_tree->setMinimumWidth(180);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tree, &QTreeView::clicked, this, &MainWindow::treeClick);
    connect(m_tree, &QTreeView::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);

    // --- View stack ---
    m_viewStack = new QStackedWidget;

    m_listView = new QListView;
    m_listView->setModel(m_proxy);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setIconSize(QSize(48, 48));
    m_listView->setGridSize(QSize(80, 80));
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setWordWrap(true);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_listView->setDragEnabled(false);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listView, &QListView::doubleClicked, this, &MainWindow::listDoubleClick);
    connect(m_listView, &QListView::customContextMenuRequested, this, &MainWindow::showListContextMenu);

    m_tableView = new QTableView;
    m_tableView->setModel(m_proxy);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSortingEnabled(true);
    m_tableView->verticalHeader()->setDefaultSectionSize(22);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setShowGrid(false);
    m_tableView->setDragEnabled(false);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::listDoubleClick);
    connect(m_tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showListContextMenu);

    m_viewStack->addWidget(m_listView);
    m_viewStack->addWidget(m_tableView);
    m_viewStack->setCurrentIndex(0);

    splitter->addWidget(m_tree);
    splitter->addWidget(m_viewStack);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
}

void MainWindow::connectSelectionSignals()
{
    connect(m_listView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::selectionChanged);
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::selectionChanged);
}

void MainWindow::setupToolbar()
{
    auto *bar = new QToolBar("Navigation", this);
    bar->setIconSize({16, 16});
    addToolBar(bar);

    auto mkact = [&](const QString &text, const QString &tip, void (MainWindow::*slot)()) {
        auto *a = bar->addAction(text);
        a->setToolTip(tip);
        connect(a, &QAction::triggered, this, slot);
        return a;
    };

    m_actBack     = mkact("◀",   "Back (Alt+Left)",    &MainWindow::goBack);
    m_actForward  = mkact("▶",   "Forward (Alt+Right)", &MainWindow::goForward);
    m_actUp       = mkact("▲",   "Up (Backspace)",      &MainWindow::goUp);
    m_actHome     = mkact("⌂",   "Home (Alt+Home)",     &MainWindow::goHome);
    m_actRefresh  = mkact("⟳",   "Refresh (F5)",        &MainWindow::refresh);

    bar->addSeparator();

    // Address bar
    m_addressBar = new QLineEdit;
    m_addressBar->setPlaceholderText("Path…");
    m_addressBar->setMinimumWidth(300);
    auto *completer = new QCompleter(m_addressBar);
    auto *fsModel = new QFileSystemModel(completer);
    fsModel->setRootPath("/");
    completer->setModel(fsModel);
    completer->setCompletionRole(QFileSystemModel::FileNameRole);
    m_addressBar->setCompleter(completer);
    connect(m_addressBar, &QLineEdit::returnPressed, this, &MainWindow::addressActivated);
    bar->addWidget(m_addressBar);

    bar->addSeparator();

    m_actNewFile   = mkact("📄+", "New File",           &MainWindow::newFile);
    m_actNewFolder = mkact("📁+", "New Folder (F7)",   &MainWindow::newFolder);
    m_actDelete    = mkact("🗑",  "Delete (Del)",       &MainWindow::deleteFiles);
    m_actRename    = mkact("✎",   "Rename (F2)",        &MainWindow::renameFile);

    bar->addSeparator();

    m_actCopy      = mkact("📋",  "Copy (Ctrl+C)",      &MainWindow::copyFiles);
    m_actCut       = mkact("✂",   "Cut (Ctrl+X)",       &MainWindow::cutFiles);
    m_actPaste     = mkact("📄",  "Paste (Ctrl+V)",     &MainWindow::pasteFiles);

    // View mode combo
    auto *viewCombo = new QComboBox;
    viewCombo->addItems({"Icons", "Details"});
    viewCombo->setCurrentIndex(0);
    connect(viewCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::setViewMode);
    bar->addWidget(viewCombo);

    updateNavButtons();
}

void MainWindow::setupMenus()
{
    auto *file = menuBar()->addMenu("&File");

    file->addAction(m_actNewFile);
    file->addAction(m_actNewFolder);
    file->addSeparator();
    file->addAction(m_actDelete);
    file->addAction(m_actRename);
    file->addSeparator();
    file->addAction(m_actCopy);
    file->addAction(m_actCut);
    file->addAction(m_actPaste);
    file->addSeparator();
    file->addAction("&Quit", QKeySequence::Quit, this, &QWidget::close);

    auto *view = menuBar()->addMenu("&View");
    m_actHidden = view->addAction("Show &Hidden Files");
    m_actHidden->setCheckable(true);
    connect(m_actHidden, &QAction::toggled, this, &MainWindow::toggleHidden);
    view->addSeparator();
    view->addAction("&Icons",    [this]{ setViewMode(0); });
    view->addAction("&Details",  [this]{ setViewMode(1); });
    view->addSeparator();
    view->addAction("Re&fresh", QKeySequence::Refresh, this, &MainWindow::refresh);

    auto *nav = menuBar()->addMenu("&Go");
    nav->addAction(m_actBack);
    nav->addAction(m_actForward);
    nav->addAction(m_actUp);
    nav->addAction(m_actHome);
    nav->addSeparator();
    nav->addAction("&Root",      this, &MainWindow::goRoot);

    auto *help = menuBar()->addMenu("&Help");
    help->addAction("&About", [this]{
        QMessageBox::about(this, "QNX File Manager",
            "A simple file manager for QNX 8.0\n"
            "Built with Qt6.");
    });
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel;
    statusBar()->addWidget(m_statusLabel, 1);
    updateStatusBar();
}

// ── Navigation ──

void MainWindow::navigateTo(const QModelIndex &proxyIndex)
{
    QModelIndex src = m_proxy->mapToSource(proxyIndex);
    QString path = m_model->filePath(src);
    navigateToPath(path);
}

void MainWindow::navigateToPath(const QString &path)
{
    if (currentPath() == path) return;

    if (!m_backStack.isEmpty() && m_backStack.top() == path)
        return;

    m_backStack.push(currentPath());
    m_forwardStack.clear();

    QModelIndex src = m_model->setRootPath(path);
    QModelIndex proxy = m_proxy->mapFromSource(src);

    m_tree->setRootIndex(m_proxy->mapFromSource(m_model->index(path)));
    m_listView->setRootIndex(proxy);
    m_tableView->setRootIndex(proxy);
    m_listView->setModelColumn(0);

    QString parent = QFileInfo(path).path();
    m_tree->expand(m_proxy->mapFromSource(m_model->index(parent)));

    m_tree->setCurrentIndex(m_proxy->mapFromSource(src));
    updateAddressBar();
    updateNavButtons();
    updateStatusBar();
}

void MainWindow::treeClick(const QModelIndex &index)
{
    QModelIndex src = m_proxy->mapToSource(index);
    if (m_model->isDir(src))
        navigateToPath(m_model->filePath(src));
}

void MainWindow::listDoubleClick(const QModelIndex &index)
{
    if (!index.isValid()) return;

    QModelIndex src = m_proxy->mapToSource(index);
    if (m_model->isDir(src))
        navigateToPath(m_model->filePath(src));
    else
        openFile(index);
}

void MainWindow::openFile(const QModelIndex &index)
{
    QModelIndex src = m_proxy->mapToSource(index);
    QString path = m_model->filePath(src);
    openEditor(path);
}

void MainWindow::openEditor(const QString &path)
{
    auto *editor = new TextEditor(path);
    editor->show();
    statusBar()->showMessage("Opened: " + path, 3000);
}

QString MainWindow::currentPath() const
{
    return m_model->rootPath();
}

QStringList MainWindow::selectedFiles() const
{
    QItemSelectionModel *sel = m_viewStack->currentIndex() == 0
        ? m_listView->selectionModel()
        : m_tableView->selectionModel();

    QStringList files;
    for (const QModelIndex &idx : sel->selectedIndexes()) {
        // Only first column to avoid duplicates (details view)
        if (idx.column() > 0) continue;
        QModelIndex src = m_proxy->mapToSource(idx);
        files.append(m_model->filePath(src));
    }
    return files;
}

// ── Slots ──

void MainWindow::goUp()
{
    QString up = QFileInfo(currentPath()).path();
    navigateToPath(up);
}

void MainWindow::goBack()
{
    if (m_backStack.isEmpty()) return;
    m_forwardStack.push(currentPath());
    navigateToPath(m_backStack.pop());
}

void MainWindow::goForward()
{
    if (m_forwardStack.isEmpty()) return;
    m_backStack.push(currentPath());
    navigateToPath(m_forwardStack.pop());
}

void MainWindow::goHome()
{
    navigateToPath("/data");
}

void MainWindow::goRoot()
{
    navigateToPath("/");
}

void MainWindow::refresh()
{
    m_model->setRootPath(currentPath());
}

void MainWindow::addressActivated()
{
    QString path = m_addressBar->text();
    if (QDir(path).exists())
        navigateToPath(path);
    else
        statusBar()->showMessage("Directory not found: " + path, 3000);
}

void MainWindow::updateNavButtons()
{
    m_actBack->setEnabled(!m_backStack.isEmpty());
    m_actForward->setEnabled(!m_forwardStack.isEmpty());
}

void MainWindow::updateAddressBar()
{
    m_addressBar->setText(currentPath());
}

void MainWindow::updateStatusBar()
{
    QDir dir(currentPath());
    int files = 0, dirs = 0;
    const auto entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const auto &e : entries) {
        if (e.isDir()) dirs++;
        else files++;
    }
    int sel = selectedFiles().size();
    QString text = QString("%1  |  %2 dirs, %3 files").arg(currentPath()).arg(dirs).arg(files);
    if (sel > 0)
        text += QString("  |  %1 selected").arg(sel);
    m_statusLabel->setText(text);
}

void MainWindow::selectionChanged()
{
    updateStatusBar();
}

// ── File operations ──

void MainWindow::newFolder()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Folder", "Name:", QLineEdit::Normal, "New Folder", &ok);
    if (!ok || name.isEmpty()) return;

    QDir dir(currentPath());
    if (dir.mkdir(name))
        refresh();
    else
        QMessageBox::warning(this, "Error", "Could not create folder.");
}

void MainWindow::deleteFiles()
{
    QStringList files = selectedFiles();
    if (files.isEmpty()) return;

    auto ret = QMessageBox::question(this, "Delete",
        QString("Delete %1 item(s)?").arg(files.size()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    for (const QString &f : files) {
        QFileInfo fi(f);
        if (fi.isDir()) {
            QDir(f).removeRecursively();
        } else {
            QFile::remove(f);
        }
    }
    refresh();
}

void MainWindow::renameFile()
{
    QStringList files = selectedFiles();
    if (files.isEmpty()) return;
    if (files.size() > 1) {
        statusBar()->showMessage("Select only one file to rename", 3000);
        return;
    }

    QFileInfo fi(files.first());
    bool ok;
    QString name = QInputDialog::getText(this, "Rename", "New name:",
        QLineEdit::Normal, fi.fileName(), &ok);
    if (!ok || name.isEmpty() || name == fi.fileName()) return;

    QDir dir = fi.dir();
    if (dir.rename(fi.fileName(), name))
        refresh();
    else
        QMessageBox::warning(this, "Error", "Could not rename.");
}

void MainWindow::showProperties()
{
    QStringList files = selectedFiles();
    if (files.isEmpty()) return;

    QString info;
    for (const QString &f : files) {
        QFileInfo fi(f);
        info += QString("Name: %1\n").arg(fi.fileName());
        info += QString("Path: %1\n").arg(fi.absolutePath());
        info += QString("Size: %1 bytes\n").arg(fi.size());
        info += QString("Modified: %1\n").arg(fi.lastModified().toString("yyyy-MM-dd hh:mm"));
        info += QString("Type: %1\n\n").arg(fi.isDir() ? "Directory" : fi.suffix());
    }

    QMessageBox::information(this, "Properties", info.trimmed());
}

void MainWindow::copyFiles()
{
    m_clipboardFiles = selectedFiles();
    m_clipboardCut = false;
    statusBar()->showMessage(
        QString("Copied %1 item(s) to clipboard").arg(m_clipboardFiles.size()), 3000);
}

void MainWindow::pasteFiles()
{
    pasteFromClipboard();
}

static void cpDir_(const QString &src, const QString &dst)
{
    QDir().mkpath(dst);
    QDir srcDir(src);
    for (const QFileInfo &e : srcDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        QString s = e.absoluteFilePath();
        QString d = dst + "/" + e.fileName();
        if (e.isDir())
            cpDir_(s, d);
        else
            QFile::copy(s, d);
    }
}

void MainWindow::pasteFromClipboard()
{
    if (m_clipboardFiles.isEmpty()) {
        statusBar()->showMessage("Clipboard empty", 3000);
        return;
    }

    QString dest = currentPath();
    int copied = 0;
    for (const QString &src : m_clipboardFiles) {
        QFileInfo fi(src);
        QString dst = dest + "/" + fi.fileName();
        if (QFile::exists(dst)) continue;

        if (fi.isDir()) {
            cpDir_(src, dst);
        } else {
            QFile::copy(src, dst);
        }
        copied++;
    }

    if (m_clipboardCut) {
        for (const QString &src : m_clipboardFiles)
            QFile::remove(src);
        m_clipboardFiles.clear();
    }

    refresh();
    statusBar()->showMessage(QString("Pasted %1 item(s)").arg(copied), 3000);
}

void MainWindow::setViewMode(int mode)
{
    m_viewMode = mode;
    m_viewStack->setCurrentIndex(mode);
}

void MainWindow::toggleHidden()
{
    if (m_actHidden->isChecked())
        m_proxy->setFilterRegularExpression(QRegularExpression());
    else
        m_proxy->setFilterRegularExpression(QRegularExpression("^(?!\\.)"));
    refresh();
}

// ── Context menus ──

void MainWindow::showTreeContextMenu(const QPoint &pos)
{
    QModelIndex idx = m_tree->indexAt(pos);
    if (!idx.isValid()) return;

    QModelIndex src = m_proxy->mapToSource(idx);
    bool isDir = m_model->isDir(src);
    m_tree->setCurrentIndex(idx);
    navigateToPath(m_model->filePath(src));

    buildContextMenu(m_tree->viewport()->mapToGlobal(pos), isDir);
}

void MainWindow::showListContextMenu(const QPoint &pos)
{
    QModelIndex idx;
    bool isDir = false;
    QPoint globalPos;

    if (auto *lv = qobject_cast<QListView*>(sender())) {
        idx = lv->indexAt(pos);
        globalPos = lv->viewport()->mapToGlobal(pos);
    } else if (auto *tv = qobject_cast<QTableView*>(sender())) {
        idx = tv->indexAt(pos);
        globalPos = tv->viewport()->mapToGlobal(pos);
    } else {
        return;
    }

    if (!idx.isValid()) {
        buildContextMenu(globalPos, false);
        return;
    }

    QModelIndex src = m_proxy->mapToSource(idx);
    isDir = m_model->isDir(src);

    QItemSelectionModel *sel = m_viewStack->currentIndex() == 0
        ? m_listView->selectionModel()
        : m_tableView->selectionModel();

    if (!sel->isSelected(idx)) {
        sel->clearSelection();
        sel->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    buildContextMenu(globalPos, isDir);
}

QMenu * MainWindow::buildContextMenu(const QPoint &globalPos, bool isDir)
{
    QMenu menu;

    if (isDir) {
        menu.addAction("Open", [this]{
            QStringList sel = selectedFiles();
            if (!sel.isEmpty() && QFileInfo(sel.first()).isDir())
                navigateToPath(sel.first());
        });
    } else {
        menu.addAction("Open in Editor", [this]{
            QStringList sel = selectedFiles();
            if (!sel.isEmpty())
                openEditor(sel.first());
        });
    }

    menu.addSeparator();

    QMenu *newMenu = menu.addMenu("New");
    newMenu->addAction("File",   this, &MainWindow::newFile);
    newMenu->addAction("Folder", this, &MainWindow::newFolder);

    menu.addSeparator();

    menu.addAction(m_actCopy);
    menu.addAction(m_actCut);
    menu.addAction(m_actPaste);

    menu.addSeparator();

    menu.addAction(m_actRename);
    menu.addAction(m_actDelete);

    menu.addSeparator();

    menu.addAction("Properties", this, &MainWindow::showProperties);

    return menu.exec(globalPos) ? nullptr : nullptr;
}

void MainWindow::newFile()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New File", "File name:",
        QLineEdit::Normal, "newfile.txt", &ok);
    if (!ok || name.isEmpty()) return;

    QDir dir(currentPath());
    QString path = dir.filePath(name);
    QFile f(path);
    if (f.exists()) {
        QMessageBox::warning(this, "Error", "File already exists.");
        return;
    }
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", f.errorString());
        return;
    }
    f.close();
    refresh();
    openEditor(path);
}

void MainWindow::cutFiles()
{
    m_clipboardFiles = selectedFiles();
    m_clipboardCut = true;
    statusBar()->showMessage(
        QString("Cut %1 item(s) to clipboard").arg(m_clipboardFiles.size()), 3000);
}

void MainWindow::copyToClipboard(const QStringList &files, bool cut)
{
    m_clipboardFiles = files;
    m_clipboardCut = cut;
}

MainWindow::~MainWindow()
{
    // Child windows (editors) close automatically with WA_DeleteOnClose
}

// ── Keyboard ──

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Backspace:
        goUp();
        break;
    case Qt::Key_F2:
        renameFile();
        break;
    case Qt::Key_F5:
        refresh();
        break;
    case Qt::Key_F7:
        newFolder();
        break;
    case Qt::Key_Delete:
        deleteFiles();
        break;
    case Qt::Key_Return:
        if (!event->modifiers()) {
            QStringList sel = selectedFiles();
            if (sel.size() == 1) {
                QFileInfo fi(sel.first());
                if (fi.isDir())
                    navigateToPath(fi.absoluteFilePath());
            }
        }
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}
