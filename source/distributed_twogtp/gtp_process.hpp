#ifndef GTP_PROCESS_H_
#define GTP_PROCESS_H_

#include <QString>
#include <QProcess>
#include "player.hpp"
#include "vertex.hpp"
#include "move.hpp"


class GtpProcess {
public:
  GtpProcess ();
  ~GtpProcess ();

  bool Start (QString name,
              QString command_line,
              int timeout_ms = 20000);

  bool IsRunning ();
  
  QString Name () const;
  QString CommandLine () const;

  bool ClearBoard (uint board_size, float komi);
  bool Play (Move);
  bool Genmove (Player, Vertex*, int timeout_ms);
  bool Quit ();

  QVariant TryCommand (QString command);

private:
  bool Command (QString command, QString* response, int timeout_ms = 1000);
  bool BasicCommandsOk ();

private:
  QProcess process;
  QString name;
  QString command_line;
};


#endif
