#pragma once

#include <QMainWindow>

class QPlainTextEdit;

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(const QString &filePath = {}, QWidget *parent = nullptr);

    QString filePath() const { return m_filePath; }
    bool save();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onModified();
    bool maybeSave();
    bool saveAs();

private:
    void loadFile();
    void updateTitle();

    QString m_filePath;
    QPlainTextEdit *m_edit;
};
