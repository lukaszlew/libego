#ifndef _GTP_H_
#define _GTP_H_

// ----------------------------------------------------------------------

class GtpResult {
public:
  GtpResult ();
  static GtpResult success (string response = "");
  static GtpResult failure (string response = "");
  static GtpResult syntax_error ();
  static GtpResult quit ();

  bool quit_loop ();
  string to_string ();

private:
  enum Status {
    status_success,
    status_failure,
    status_quit
  };

  string status_marker ();
  GtpResult (Status status, string response="");

  Status status_;
  string response_;
};

// ----------------------------------------------------------------------

class GtpCommand {
public:
  virtual GtpResult exec_command (string command_name, istream& params) = 0;
};


// ----------------------------------------------------------------------

class GoguiParam {
public:
  static GoguiParam String (string name, string* ptr);
  static GoguiParam Float (string name, float* ptr);
  static GoguiParam Uint (string name, uint* ptr);
  static GoguiParam Bool (string name, bool* ptr);

  string gogui_string ();
  string type_to_string ();
  string value_to_string ();
  bool set_value(istream& in);

  string name; 

  // emulation of sum type
  enum Type {
    type_string,
    type_float,
    type_uint,
    type_bool,
  } type;
  
  union {
    string* string_param;
    float*  float_param;
    uint*   uint_param;
    bool*   bool_param;
  };
};

// ----------------------------------------------------------------------

class Gtp : public GtpCommand {
public:
  Gtp ();

  bool is_command (string name);
  bool is_static_command (string name);
  bool is_gogui_param_command (string name);
  void add_gtp_command (GtpCommand* command, string name);
  void add_static_command (string name, string response);
  void extend_static_command (string name, string response_ext);
  void add_gogui_command (GtpCommand* command, string type, string name,
                          string params, bool extend_only_if_new = false);
  void add_gogui_param_string (string cmd_name, string param_name, string* ptr);
  void add_gogui_param_float (string cmd_name, string param_name, float* ptr);
  void add_gogui_param_uint (string cmd_name, string param_name, uint* ptr);
  void add_gogui_param_bool (string cmd_name, string param_name, bool* ptr);

  bool run_file (string file_name, ostream& out = cout);
  void run_loop (istream& in = cin, ostream& out = cout, bool echo_commands = false);

  virtual GtpResult exec_command (string command, istream& params);

private:

  void preprocess (string* s);

  map <string, GtpCommand*>          command_of_name;
  map <string, string>               command_to_response;
  map <string, vector<GoguiParam> >  command_to_gogui_params;
};

#endif
