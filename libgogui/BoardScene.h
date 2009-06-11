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
  BoardScene(int size, QObject *parent = 0);

  virtual QGraphicsItem* addBlackStone(const QString& pos);
  virtual QGraphicsItem* addWhiteStone(const QString& pos);
  void removeStone(const QString& pos);

  QGraphicsItem* addMark(const QString& pos);
  void removeMark(const QString& pos);

  QGraphicsItem* addCircle(const QString& pos);
  void removeCircle(const QString& pos);

  QGraphicsItem* addSquare(const QString& pos);
  void removeSquare(const QString& pos);

  QGraphicsItem* addTriangle(const QString& pos);
  void removeTriangle(const QString& pos);

  virtual QGraphicsItem* addLabel(const QString& pos, const QString& label);
  void removeLabel(const QString& pos);

  static QString getFieldString(int x, int y);

signals:
  void fieldClicked(const QString& pos, Qt::MouseButtons buttons);

protected slots:
  void debugClick(const QString& pos, Qt::MouseButtons buttons);

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
  virtual QGraphicsItem* addGrid() = 0;
  virtual QGraphicsItem* addRuler() = 0;
  virtual QGraphicsItem* addShape(const QString& pos, EShapeType type);
  virtual void removeShape(const QString& pos, EShapeType type);

  class locker {
  public:
    locker(QMutex& mutex) : m_mutex(mutex) { m_mutex.lock(); }
    ~locker() {m_mutex.unlock(); }
  private:
    QMutex& m_mutex;
  };

  QMutex m_mutex;

  const int m_size;
  typedef QMap<QString, Field*> map_type;
  map_type m_fields;
  Grid *m_grid; // not-null
  QGraphicsItem *m_ruler;
};

#endif /* BOARDSCENE_H_ */
