#include "editor.h"

#include <QPlainTextEdit>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSaveFile>
#include <QMenuBar>
#include <QAction>
#include <QApplication>
#include <QKeySequence>
#include <QFont>
#include <QVBoxLayout>
#include <QCloseEvent>

TextEditor::TextEditor(const QString &filePath, QWidget *parent)
    : QMainWindow(parent), m_filePath(filePath)
{
    resize(700, 500);
    setAttribute(Qt::WA_DeleteOnClose);

    m_edit = new QPlainTextEdit(this);
    m_edit->setFont(QFont("DejaVu Sans Mono", 10));
    m_edit->setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    setCentralWidget(m_edit);

    connect(m_edit, &QPlainTextEdit::modificationChanged, this, &TextEditor::onModified);

    // File menu
    auto *file = menuBar()->addMenu("&File");
    file->addAction("&Save",   QKeySequence::Save,   this, &TextEditor::save);
    file->addAction("Save &As…", QKeySequence("Ctrl+Shift+S"), this, &TextEditor::saveAs);
    file->addSeparator();
    file->addAction("&Close", QKeySequence::Close, this, &QWidget::close);

    if (!m_filePath.isEmpty())
        loadFile();

    updateTitle();
}

void TextEditor::loadFile()
{
    QFile f(m_filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", f.errorString());
        return;
    }
    m_edit->setPlainText(f.readAll());
    m_edit->document()->setModified(false);
    f.close();
}

bool TextEditor::save()
{
    if (m_filePath.isEmpty())
        return saveAs();

    QSaveFile f(m_filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", f.errorString());
        return false;
    }
    f.write(m_edit->toPlainText().toUtf8());
    if (!f.commit()) {
        QMessageBox::warning(this, "Error", f.errorString());
        return false;
    }
    m_edit->document()->setModified(false);
    updateTitle();
    return true;
}

bool TextEditor::saveAs()
{
    QMessageBox::information(this, "Save As",
        "Save As not available on QNX.\n"
        "Use the file manager to copy/rename the file.");
    return false;
}

void TextEditor::onModified()
{
    updateTitle();
}

bool TextEditor::maybeSave()
{
    if (!m_edit->document()->isModified())
        return true;

    auto ret = QMessageBox::warning(this, "Unsaved Changes",
        QString("'%1' has been modified.\nSave changes?")
            .arg(QFileInfo(m_filePath).fileName()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return save();
    return ret == QMessageBox::Discard;
}

void TextEditor::updateTitle()
{
    QString name = m_filePath.isEmpty()
        ? "Untitled"
        : QFileInfo(m_filePath).fileName();
    if (m_edit && m_edit->document()->isModified())
        name += " *";
    setWindowTitle(name + " - QNX Text Editor");
}

void TextEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}
