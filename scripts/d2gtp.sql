DROP TABLE IF EXISTS game;
DROP TABLE IF EXISTS experiment;
DROP TABLE IF EXISTS game_setup;
DROP TABLE IF EXISTS engine;
DROP TABLE IF EXISTS param;
DROP TABLE IF EXISTS engine_param;


CREATE TABLE engine (
  id INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL UNIQUE,

  command_line TEXT NOT NULL,
  gtp_name TEXT,                -- to be checked upon execution
  gtp_version TEXT,             -- to be checked upon execution
  send_gtp_config TEXT NOT NULL -- first commands sent to engine
);


CREATE TABLE game_setup (
  id INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL UNIQUE,

  rule_set TEXT NOT NULL,       -- will be sent to engine with go_rules GTP command
  board_size INTEGER NOT NULL,
  komi REAL NOT NULL
);


CREATE TABLE experiment (
  id INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT,

  game_setup_id INTEGER NOT NULL REFERENCES game_setup (id),

  first_engine_id INTEGER NOT NULL REFERENCES engine (id),
  second_engine_id INTEGER NOT NULL REFERENCES engine (id),

  description TEXT
);


CREATE TABLE game (
  id INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT,

  experiment_id INTEGER NOT NULL REFERENCES experiment (id),
  first_is_black INTEGER NOT NULL,  

  -- text returned by game_report GTP command
  first_report TEXT,
  second_report TEXT,

  -- report
  first_won INTEGER,
  sgf TEXT,

  -- job scheduling
  created_at DATETIME NOT NULL,
  claimed_at DATETIME,
  claimed_by TEXT,
  finished_at DATETIME
);


CREATE TABLE param (
  id INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT,
  experiment_id INTEGER NOT NULL REFERENCES game (id),
  name TEXT NOT NULL
);


CREATE TABLE engine_param (
  game_id INTEGER NOT NULL REFERENCES game (id),
  name TEXT NOT NULL,
  value TEXT NOT NULL
);
