#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QMap>
#include <QFile>
#include <iostream>
#include <QTextStream>
#include <QString>
#include <QSaveFile>
#include <QFileDialog>
#include <QDataStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray MainWindow::GetNum1(){
        QFile file1(fileNameIn1);//Файл 1
        file1.open(QIODevice::ReadOnly);
        QByteArray num1 = file1.readAll();
        QFile file2(fileNameIn2);//Файл 2
        file1.open(QIODevice::ReadOnly);
        QByteArray num2 = file2.readAll();

        for(int i = num1.size(); i<num2.size();i++){ //Расширяем num1 чтобы мы сделали его размером num2 и цикл не выходил за массив при работе с ним
              num1.append('0');
          }
        file1.close();
        file2.close();
    return num1;
}
void MainWindow::CreatePatch(){
    QFile filePatch("patch.dat");
    QFileInfo filePatchInfo(filePatch);

    QByteArray num1 = GetNum1(); //Читаем их в переменные QByteArray

    QFile file2(fileNameIn2);//Файл 2

    file2.open(QIODevice::ReadOnly);


    QByteArray num2 = file2.readAll();

    int maxArray = std::max(num1.size(),num2.size()); //Понадобится при прохождении по циклу

    if (filePatch.open(QIODevice::WriteOnly)){
        QDataStream out(&filePatch);
        for(int i = 0;i<maxArray;i++) //заполняем контейнер различиями в файлах, key - позиция, value - значение, параллельно создаем документ patch
        {
            if(num1[i]!=num2[i]){
                out << i;
                out << num2[i];
            }
        }

    }

    patchFileName = filePatchInfo.absoluteFilePath();
    ui->patchFileLabel->setText(patchFileName);

    filePatch.close();
    file2.close();
}


void MainWindow::FilePatcher(){
    QFile filePatch(patchFileName); //Файл с патчем
    QFile file1(fileNameIn1);//Файл 1

    QSaveFile file1ToSave(fileNameIn1);//Чтобы сохранить файл в бинарном формате, надо его открыть с помощью QSaveFile

    QByteArray num1 = GetNum1();

    if (filePatch.open(QIODevice::ReadOnly)){ //Открываем ещё раз filePatch, но уже для чтения
        QDataStream in(&filePatch);
            while(!in.atEnd()){
            int key;
            char value;
            in >> key;
            in >> value;
            num1[key] = value;
            }
    }

    else qDebug()<< "Cant open file";

        filePatch.close();

    if (file1ToSave.open(QIODevice::WriteOnly)){ //Сохраняем весь наш QByteArray в файл
        file1ToSave.write(num1);
        file1ToSave.commit();
    }



}
void MainWindow::on_file1Button_clicked()
{
    fileNameIn1 = QFileDialog::getOpenFileName();
    ui ->file1Label->setText(fileNameIn1);
}


void MainWindow::on_file2Button_clicked()
{
    fileNameIn2 = QFileDialog::getOpenFileName();
    ui->file1Label_2->setText(fileNameIn2);
}

void MainWindow::on_pushButton_clicked()
{
    CreatePatch();
}

void MainWindow::on_workButton_clicked()
{
    FilePatcher();
}



void MainWindow::on_pushButton_2_clicked()
{
    patchFileName = QFileDialog::getOpenFileName();
    ui->patchFileLabel->setText(patchFileName);
}

