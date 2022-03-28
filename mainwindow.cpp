//Часов убито на то, чтобы разобраться в QBitArray: 7
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
#include <QBitArray>
#include <QDebug>

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

QByteArray MainWindow::GetFile1ByteArray(){
        QFile file1ToPatch(fileNameIn1);//Файл 1
        QFile file2GetDiff(fileNameIn2);//Файл 2

        file1ToPatch.open(QIODevice::ReadOnly); //открываем файлы
        file2GetDiff.open(QIODevice::ReadOnly);

        QByteArray file1ByteArray = file1ToPatch.readAll();//читаем данные, записываем в массив
        QByteArray file2ByteArray = file2GetDiff.readAll();
        for(int i = file1ByteArray.size(); i<file2ByteArray.size();i++){ //на самом деле этот цикл не до конца нужен, оставил её на всякий случай с тестов, исходные файлы в нашем случае одинакового размера
              file1ByteArray.append('0'); //если не одинакого - массив первый дополняется нулями для того, чтобы ниже не выходить за массив
          }
        file1ToPatch.close();
        file2GetDiff.close();

    return file1ByteArray; //возвращаем массив байтов
}

void MainWindow::CreatePatch(){
    QFile filePatch("patch.dat"); //файл патча
    QFile file2GetDiff(fileNameIn2); // файл2
    QFileInfo filePatchInfo(filePatch); //берем инфу для патча, понадобится для вывода пути патча на экран

    file2GetDiff.open(QIODevice::ReadOnly);

    QByteArray file1ByteArray = GetFile1ByteArray(); //заполняем массив из функции выше
    QByteArray file2ByteArray = file2GetDiff.readAll(); //читаем второй файл

    int maxArray = std::max(file1ByteArray.size(),file2ByteArray.size()); //вычисляем максимальный размер, опять таки, у нас экземплы одного размера поэтому сделано на всякий случай для if ниже

    /*создаем массив битов, в него будем записывать положения с различиями в файлах
      к примеру, файл а: 12345, файл б: 12335, массив битов будет записан как 00010, где 1 - положение элемента, который надо будет заменить при применении патча*/
    QBitArray bitArrayToPatchFile(maxArray);

    if (filePatch.open(QIODevice::WriteOnly)){
        QByteArray diffDataOut; //временный массив, сначала заполним его, параллельно заменяя 0 на 1 в bitArray, потом выведем
        QDataStream out(&filePatch);
        for(int i = 0;i<maxArray;i++)
            {
                if(file1ByteArray[i]!=file2ByteArray[i]){
                    bitArrayToPatchFile.setBit(i,true);
                    diffDataOut.append(file2ByteArray[i]); //заполняем временный массив элементами, которые надо будет вставить в файл1
                }
        }
        out << bitArrayToPatchFile; //выводим данные, обязательно сначала выводим битовый массив, потом массив элементов
        out << diffDataOut;
    }

    patchFileName = filePatchInfo.absoluteFilePath();
    ui->patchFileLabel->setText(patchFileName);//выводим путь патча на экран

    filePatch.close();
    file2GetDiff.close();
}


void MainWindow::FilePatcher(){
    QFile filePatch(patchFileName); //Файл с патчем
    QFile file1ToPatch(fileNameIn1);//Файл 1

    QSaveFile file1Out(fileNameIn1);//Чтобы сохранить файл в бинарном формате, надо его открыть с помощью QSaveFile
    QByteArray diffDataIn;//в этот массив впишем символы из патча
    file1ToPatch.open(QIODevice::ReadOnly);
    QByteArray file1ByteArray = file1ToPatch.readAll(); //читаем файл1, заносим все в массив
    file1ToPatch.close();
    QBitArray bitArrayIn(file1ByteArray.length()); //объявляем массив битов, в него внесем биты из патча

    //счетчик для массива символов, если нам надо заменить символ, мы заменяем его и добавляем +1 к счетчику, чтобы когда надо заменить следующий символ, мы взяли его из следующего элемента массива байтов
    int byteArrayCount=0;

    if (filePatch.open(QIODevice::ReadOnly)){ //Открываем ещё раз filePatch, но уже для чтения
        QDataStream in(&filePatch);

        while(!in.atEnd()){
            in >> bitArrayIn; //читаем массив битов из файла
            in >> diffDataIn; //читаем массив байтов из файла
            for(int i =0; i<bitArrayIn.size();i++){
                if(bitArrayIn[i]!=false){ //если элемент равен 1, то мы по его положению в массиве битов заменяем на таком же положении в файле элемент
                    file1ByteArray[i] = diffDataIn[byteArrayCount]; //производим замену
                    byteArrayCount++; // |a|bcd -> a|b|cd
                }
            }
        }
    }

    filePatch.close();

    if (file1Out.open(QIODevice::WriteOnly)){ //Сохраняем весь наш QByteArray в файл
        file1Out.write(file1ByteArray);
        file1Out.commit();
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

