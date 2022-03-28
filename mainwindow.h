#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString fileNameIn1,fileNameIn2,patchFileName; //названия файлов для вывода их на экран приложения

    void FilePatcher();//Наша функция
    void CreatePatch();
    QByteArray GetFile1ByteArray();



private:
    Ui::MainWindow *ui;

private slots:
    void on_workButton_clicked();
    void on_file1Button_clicked();
    void on_file2Button_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};
#endif // MAINWINDOW_H
