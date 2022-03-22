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

void MainWindow::FilePatcher(){

    QMap<int,char> differenceBetweenFiles; //Map, в него мы будем заносить int - положение отличия между файлами и char - самое отличие

    QFile filePatch("patch.txt"); //Файл с патчем
    QFile file1(fileNameIn1);//Файл 1
    QFile file2(fileNameIn2);//Файл 2

    QSaveFile file1ToSave(fileNameIn1);//Чтобы сохранить файл в бинарном формате, надо его открыть с помощью QSaveFile


    file1.open(QIODevice::ReadWrite); //Открываем файлы
    file2.open(QIODevice::ReadOnly);

    QByteArray num1 = file1.readAll(); //Читаем их в переменные QByteArray
    QByteArray num2 = file2.readAll();

    int maxArray = std::max(num1.size(),num2.size()); //Понадобится при прохождении по циклу

    for(int i = num1.size(); i<num2.size();i++){ //Расширяем num1 чтобы мы сделали его размером num2 и цикл не выходил за массив при работе с ним
          num1.append('0');
      }

    if (filePatch.open(QIODevice::WriteOnly)){
        QTextStream out(&filePatch);
        for(int i = 0;i<maxArray;i++) //заполняем контейнер различиями в файлах, key - позиция, value - значение, параллельно создаем документ patch
        {
            if(num1[i]!=num2[i]){

                differenceBetweenFiles.insert(i,num2[i]);//Выводим все это в файл fiilePatch(patch.txt)
                out << i << ":" << num2[i] << "\n";
            }
        }
        filePatch.close();
    }

    if (filePatch.open(QIODevice::ReadOnly)){ //Открываем ещё раз filePatch, но уже для чтения
        QTextStream in(&filePatch);
        while (!in.atEnd()) {
            QString line = in.readLine(); //Читаем файл патча
            //В патчере данные внесены в формат вида 0:a, где 0 - позиция в файле, которая отличается, и a - сам отличающийся элемент. Они разделены :
            keyFromFile = line.section(':',0,0); //Берем первый элемент и пишем его в как позицию в файле
            valueFromFile = line.section(':',1,1); //Берем второй элемент и пишем его как элемент, который будем вставлять

            keyBuf = keyFromFile.toInt();//Переводим в int чтобы QByteArray мог распознать позицию по числу

            //QByteArray не умеет работать с char и QString, поэтому тут будет небольшой костыль. Мы берем и переводим переменную в std::string
            //И затем перекидываем её в char, так как QString не переносится в char, зато он переносится в std::string, что можно перевести в нужный
            //для работы с QByteArray char
            tempValueFromFile= valueFromFile.toStdString();
            charTempValueFromFile = tempValueFromFile[0];

            num1[keyBuf] = charTempValueFromFile;//Производим замену
          }
    }

    else qDebug()<< "Cant open file";

    file1.close();
    file2.close();

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

void MainWindow::on_workButton_clicked()
{
    FilePatcher();
}
