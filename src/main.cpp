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
    
  int pos = cmd.find(' ');
  string mainCmd = cmd.substr(0, pos);
  string restCmd = cmd.substr(pos + 1);

  if(cmd == "exit"){
    break;
  } else if (cmd == "echo"){
    cout << "" << endl;
  } else if (mainCmd == "echo") {
    cout << restCmd << endl;
  } else {
    cout << cmd << ": command not found" << endl; 
  }
  }
}
