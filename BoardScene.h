#ifndef BOARDSCENE_H_
#define BOARDSCENE_H_

#include <QGraphicsScene>
#include <QMap>

class Field;

class BoardScene : public QGraphicsScene
{
  Q_OBJECT

public:
  BoardScene(int size, QObject *parent = 0);
  virtual ~BoardScene();

  virtual QGraphicsItem* addBlackStone(const QString& pos);
  virtual QGraphicsItem* addWhiteStone(const QString& pos);
  virtual void removeStone(const QString& pos);

  virtual QGraphicsItem* addMark(const QString& pos, QColor color);
  virtual void removeMark(const QString& pos);

  virtual QGraphicsItem* addCircle(const QString& pos, QColor color);
  virtual void removeCircle(const QString& pos);

  virtual QGraphicsItem* addSquare(const QString& pos, QColor color);
  virtual void removeSquare(const QString& pos);

  virtual QGraphicsItem* addTriangle(const QString& pos, QColor color);
  virtual void removeTriangle(const QString& pos);

  virtual QGraphicsItem* addLabel(const QString& pos, const QString& label);
  virtual void removeLabel(const QString& pos);

  virtual QGraphicsItem* addGrid() = 0;

  virtual QGraphicsItem* addRuler() = 0;
  virtual void removeRuler();

  static QString getFieldString(int x, int y);

signals:
  void fieldClicked(const QString& pos, Qt::MouseButtons buttons);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

  const int m_size;

  typedef QMap<QString, Field*> map_type;
  map_type m_fields;
  QGraphicsItem *m_grid;
  QGraphicsItem *m_ruler;

};

#endif /* BOARDSCENE_H_ */
