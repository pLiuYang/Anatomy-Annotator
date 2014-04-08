#include "editlesson.h"
#include <QtGui/QtGui>

EditLesson::EditLesson(QWidget* parent) : QDialog(parent)
{
    qDebug() << "4";
    textEdit = new TextEdit;
    qDebug() << "5";
}

void EditLesson::setText(QString str)
{
    textEdit->setText(str);
}
