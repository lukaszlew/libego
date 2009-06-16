#ifndef BOARDSCENE_H_
#define BOARDSCENE_H_

#include <QGraphicsScene>
#include <QMap>
#include <QMutex>
#include <boost/function.hpp>

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

  typedef boost::function<void (int,int)> callback_type;
  void setLeftButtonHandler(const callback_type& handler) { m_LeftButtonHandler = handler; }
  void setRightButtonHandler(const callback_type& handler) { m_rightButtonHandler = handler; }
  void setMidButtonHandler(const callback_type& handler) { m_midButtonHandler = handler; }
  void setXButton1Handler(const callback_type& handler) { m_xButton1Handler = handler; }
  void setXButton2Handler(const callback_type& handler) { m_xButton2Handler = handler; }

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
  callback_type m_LeftButtonHandler;
  callback_type m_rightButtonHandler;
  callback_type m_midButtonHandler;
  callback_type m_xButton1Handler;
  callback_type m_xButton2Handler;
};

#endif /* BOARDSCENE_H_ */
