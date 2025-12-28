#include <iostream>
#include <string>
using namespace std;

int main() {
  // Flush after every cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  while(true){
  // TODO: Uncomment the code below to pass the first stage
  cout << "$ ";

  // Inputing users commands
  string cmd;
  getline(cin, cmd);

  if(cmd == "exit"){
    break;
  } else if (cmd.substr(0,4) == "type") {
    if(cmd.substr(5) == "echo" || cmd.substr(5) == "exit" || cmd.substr(5) == "type" ){
      cout << cmd.substr(5) << " is a shell builtin" << endl;
    } else {
      cout << cmd.substr(5) << ": not found" << endl;
    }
  } else if (cmd.substr(0,4) == "echo") {
    cout << cmd.substr(5) << endl;
  } else {
    cout << cmd << ": command not found" << endl; 
  }
  }
}
