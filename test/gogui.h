#ifndef GOGUI_H
#define GOGUI_H

#include <QDialog>

class QLineEdit;

class GoGui: public QDialog
{
Q_OBJECT

public:
  GoGui(QWidget *parent = 0);

private slots:
  void typeIndexChanged(const QString& text);

private:
  QLineEdit *labelEdit;
};

#endif // GOGUI_H
