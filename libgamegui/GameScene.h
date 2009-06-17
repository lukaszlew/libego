#ifndef BOARDSCENE_H_
#define BOARDSCENE_H_

#include <QGraphicsScene>
#include <QMap>
#include <QMutex>

class Field;
class Grid;

class GameScene: public QGraphicsScene
{
Q_OBJECT

public:
  static GameScene *createHavannahScene(int size, QObject *parent = 0);
  static GameScene *createHexScene(int size, QObject *parent = 0);
  static GameScene *createHoScene(int size, QObject *parent = 0);
  static GameScene *createGoScene(int size, QObject *parent = 0);
  static GameScene *createYScene(int size, QObject *parent = 0);

public:
  GameScene(Grid *grid, QObject *parent = 0);

  void addBlackStone(int x, int y);
  void addWhiteStone(int x, int y);
  void removeStone(int x, int y);

  void addMark(int x, int y);
  void removeMark(int x, int y);

  void addCircle(int x, int y);
  void removeCircle(int x, int y);

  void addSquare(int x, int y);
  void removeSquare(int x, int y);

  void addTriangle(int x, int y);
  void removeTriangle(int x, int y);

  void addLabel(int x, int y, const QString& label);
  void removeLabel(int x, int y);

  static QString getFieldString(int x, int y);

  void handleMousePress(int x, int y, Qt::MouseButtons buttons);
  void handleMouseDoubleClick(int x, int y, Qt::MouseButtons buttons);
  void handleMouseRelease(int x, int y, Qt::MouseButtons buttons);
  void handleWheelMove(int x, int y, int delta);
  void handleHooverEnter(int x, int y);
  void handleHooverLeave(int x, int y);

  signals:
    void mousePressed(int x, int y, Qt::MouseButtons buttons);
    void mouseDoubleClicked(int x, int y, Qt::MouseButtons buttons);
    void mouseReleased(int x, int y, Qt::MouseButtons buttons);
    void mouseWheelMoved(int x, int y, int delta);
    void hooverEntered(int x, int y);
    void hooverLeft(int x, int y);

protected:
  enum EShapeType
  {
    TypeBlackStone,
    TypeWhiteStone,
    TypeMark,
    TypeCircle,
    TypeSquare,
    TypeTriangle,
    TypeLabel,
  };
  void addShape(int x, int y, EShapeType type, const QString& label = QString());
  void removeShape(int x, int y, EShapeType type);

  QMutex m_mutex;
  Grid *m_grid; // not-null
  QGraphicsItem *m_ruler;
  typedef QMap<QString, Field*> map_type;
  map_type m_fields;
};

#endif /* BOARDSCENE_H_ */
