#ifndef BOARDSCENE_H_
#define BOARDSCENE_H_

#include <QGraphicsScene>
#include <QMap>
#include <QMutex>

class Field;
class Grid;

class BoardScene: public QGraphicsScene
{
Q_OBJECT

public:
  enum EGameType {
    GoGame,
    HexGame,
  };
  BoardScene(EGameType game, int size, QObject *parent = 0);

  void addBlackStone(const QString& pos);
  void addWhiteStone(const QString& pos);
  void removeStone(const QString& pos);

  void addMark(const QString& pos);
  void removeMark(const QString& pos);

  void addCircle(const QString& pos);
  void removeCircle(const QString& pos);

  void addSquare(const QString& pos);
  void removeSquare(const QString& pos);

  void addTriangle(const QString& pos);
  void removeTriangle(const QString& pos);

  void addLabel(const QString& pos, const QString& label);
  void removeLabel(const QString& pos);

  static QString getFieldString(int x, int y);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

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
  void addShape(const QString& pos, EShapeType type, const QString& label = QString());
  void removeShape(const QString& pos, EShapeType type);

  QMutex m_mutex;
  const int m_size;
  Grid *m_grid; // not-null
  QGraphicsItem *m_ruler;
  typedef QMap<QString, Field*> map_type;
  map_type m_fields;

  void debugClick(const QString& pos, Qt::MouseButtons buttons);
};

#endif /* BOARDSCENE_H_ */
