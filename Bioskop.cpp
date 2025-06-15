#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <limits>

using namespace std;

struct Booking {
  string seat;
  int sessionID;
};

class User {
  string username;
  string password;
  list<Booking> bookings;

public:
  User(const string& u, const string& p) : username(u), password(p) {}

  string getUsername() { return username; }
  bool checkPassword(const string& p) { return password == p; }

  void addBooking(string seat, int session) {
    bookings.push_back({seat, session});
  }

  bool cancelBooking(string seat, int session) {
    for (auto it = bookings.begin(); it != bookings.end(); ++it) {
      if (it->seat == seat && it->sessionID == session) {
          bookings.erase(it);
          return true;
      }
    }
    return false;
  }

  void showBookings() {
    if (bookings.empty()) {
        cout << " Tidak ada booking aktif\n";
    } else {
        cout << " Booking kamu:\n";
        for (auto& b : bookings) {
          cout << " Kursi: " << b.seat << ", Sesi: " << b.sessionID << "\n";
        }
    }
  }

  bool hasBooked(string seat, int session) {
    for (auto& b : bookings) {
      if (b.seat == seat && b.sessionID == session)
          return true;
    }
    return false;
  }
};

struct Seat {
  char status;
};

struct Session {
  int id;
  string filmName;
  Seat seats[5][6];
};

class Bioskop {
  vector<User*> users;
  vector<Session> sessions;

public:
  Bioskop() {
    for (int i = 1; i <= 3; i++) {
      Session s;
      s.id = i;
      s.filmName = "Film " + to_string(i);
      for (int r = 0; r < 5; r++)
        for (int c = 0; c < 6; c++)
          s.seats[r][c].status = 'O';
      sessions.push_back(s);
    }
  }
  ~Bioskop() {
    for (auto u : users)
      delete u;
  }

  User* findUser(string username) {
    for (auto u : users) {
      if (u->getUsername() == username) return u;
    }
    return nullptr;
  }

  User* addUser(string username, string password) {
    User* u = new User(username, password);
    users.push_back(u);
    return u;
  }

    
        
