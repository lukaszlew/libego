#ifndef BOARDSCENE_H_
#define BOARDSCENE_H_

#include <QGraphicsScene>
#include <QMap>
#include <QMutex>
#include <boost/function.hpp>

class Field;
class Grid;

class BoardScene: public QGraphicsScene
{
Q_OBJECT

public:
  typedef boost::function<void (int,int)> callback_type;

  BoardScene(Grid *grid, QObject *parent = 0);

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

protected:
  //void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

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

  //void debugClick(const QString& pos, Qt::MouseButtons buttons);
};

#endif /* BOARDSCENE_H_ */
