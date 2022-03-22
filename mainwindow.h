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

    QString valueFromFile, keyFromFile; //данные которые мы будем вычитывать из файла патчера
    std::string tempValueFromFile; //костыльная переменная, описывал в вписывании файла
    char charTempValueFromFile; //ещё одна костыльная переменная, туда же относится
    QString fileNameIn1,fileNameIn2; //названия файлов для вывода их на экран приложения

    int keyBuf; //переменная, которую мы будем вставлять в QByteArray, находится рядом с костылями, эта не костыльная и важная
    void FilePatcher();//Наша функция



private:
    Ui::MainWindow *ui;

private slots:
    void on_workButton_clicked();
    void on_file1Button_clicked();
    void on_file2Button_clicked();
};
#endif // MAINWINDOW_H
