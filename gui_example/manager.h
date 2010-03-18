#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include <QString>

class GameScene;

class manager : public QObject
{
  Q_OBJECT
public:
  manager(GameScene *scene, QObject * parent = 0);


public slots:
  void handleMousePress(int x, int y, Qt::MouseButtons buttons);
  void setIndex(int index) { m_index = index; }
  void setLabel(const QString& label) { m_label = label; }

private:
  int m_index;
  QString m_label;
  GameScene *m_gameScene;

};

#endif /* MANAGER_H_ */
