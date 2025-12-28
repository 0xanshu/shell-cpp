#include <iostream>
#include <string>
using namespace std;

int main() {
  // Flush after every cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  bool exit = 1;
  while(exit){
  // TODO: Uncomment the code below to pass the first stage
  cout << "$ ";

  // Inputing users commands
  string cmd;
  cin >> cmd;

  cout << cmd << ": command not found" << endl;
  }
}
