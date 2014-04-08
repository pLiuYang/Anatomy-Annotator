#ifndef EDITLESSON_H
#define EDITLESSON_H

#include <QtGui/QDialog>
#include "textedit.h"

class EditLesson : public QDialog
{
    Q_OBJECT

public:
    EditLesson(QWidget* parent = 0);
    void setText(QString str);

signals:

private slots:

private:
    TextEdit *textEdit;
};

#endif // EDITLESSON_H
