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
    QByteArray fileNameIn1QB = fileNameIn1.toLocal8Bit();
    QByteArray fileNameIn2QB = fileNameIn2.toLocal8Bit();
    const char *fileNameIn1Char = fileNameIn1QB.data();
    const char *fileNameIn2Char = fileNameIn2QB.data();

    QFile filePatch("patch.dat"); //Файл с патчем
    //QFile file1(fileNameIn1);//Файл 1
    FILE* file1;
    file1 = fopen(fileNameIn1Char,"rb");
    FILE* file2;
    file2 = fopen(fileNameIn2Char,"rb");
    //QFile file2(fileNameIn2);//Файл 2

    QSaveFile file1ToSave(fileNameIn1);//Чтобы сохранить файл в бинарном формате, надо его открыть с помощью QSaveFile
    QByteArray num1;
    QByteArray num2;
    QByteArray patchData,compressedPatch;

    //file1.open(QIODevice::ReadWrite); //Открываем файлы
    fread(&num1,num1.size(),1,file1);
    fread(&num2,num2.size(),1,file2);
    //file2.open(QIODevice::ReadOnly);


   // QByteArray num2 = file2.readAll();

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
                patchData += i;
                patchData += ":";
                patchData += num2[i];
            }
        }

        compressedPatch = qCompress(patchData,9);
        filePatch.write(compressedPatch);
        filePatch.close();
    }

    if (filePatch.open(QIODevice::ReadOnly)){ //Открываем ещё раз filePatch, но уже для чтения
        QByteArray uncompressedPatch1 = filePatch.readAll();
        QByteArray uncompressedPatch = qUncompress(uncompressedPatch1);
        while (!filePatch.atEnd()) {
            QString line = QString(uncompressedPatch); //Читаем файл патча
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
    fclose(file1);
    fclose(file2);

    fopen(fileNameIn1Char,"wb");
    fwrite(&num1,num1.size(),1,file1);
    fclose(file1);

    /*if (file1ToSave.open(QIODevice::WriteOnly)){ //Сохраняем весь наш QByteArray в файл
        file1ToSave.write(num1);
        file1ToSave.commit()
        }*/

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
