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

  void showSessions() {
        cout << "Sesi Film yang tersedia:\n";
        for (auto& s : sessions) {
            cout << "  Sesi " << s.id << ": " << s.filmName << "\n";
        }
    }

    void showSeats(int sessionID) {
        Session* s = getSession(sessionID);
        if (!s) {
            cout << "Sesi tidak ditemukan!\n";
            return;
        }
        cout << "  Kursi (O = kosong, X = booked):\n    1 2 3 4 5 6\n";
        for (int r = 0; r < 5; r++) {
            cout << " " << char('A' + r) << " ";
            for (int c = 0; c < 6; c++) {
                cout << s->seats[r][c].status << " ";
            }
            cout << "\n";
        }
    }

    bool validSeat(string seat) {
        if (seat.length() < 2 || seat.length() > 3) return false;
        char row = toupper(seat[0]);
        if (row < 'A' || row > 'E') return false;
        string colStr = seat.substr(1);
        if (!all_of(colStr.begin(), colStr.end(), ::isdigit)) return false;
        int col = stoi(colStr);
        if (col < 1 || col > 6) return false;
        return true;
    }

    bool seatAvailable(int sessionID, string seat) {
        Session* s = getSession(sessionID);
        if (!s) return false;
        int row = toupper(seat[0]) - 'A';
        int col = stoi(seat.substr(1)) - 1;
        return s->seats[row][col].status == 'O';
    }

    bool bookSeat(int sessionID, string seat) {
        Session* s = getSession(sessionID);
        if (!s) return false;
        int row = toupper(seat[0]) - 'A';
        int col = stoi(seat.substr(1)) - 1;
        if (s->seats[row][col].status == 'O') {
            s->seats[row][col].status = 'X';
            return true;
        }
        return false;
    }

    bool cancelSeat(int sessionID, string seat) {
        Session* s = getSession(sessionID);
        if (!s) return false;
        int row = toupper(seat[0]) - 'A';
        int col = stoi(seat.substr(1)) - 1;
        if (s->seats[row][col].status == 'X') {
            s->seats[row][col].status = 'O';
            return true;
        }
        return false;
    }

    Session* getSession(int id) {
        for (auto& s : sessions) {
            if (s.id == id) return &s;
        }
        return nullptr;
    }

    bool dcCariSeatKosong(int sessionID, int lowRow, int highRow, int lowCol, int highCol) {
        if (lowRow > highRow || lowCol > highCol) return false;

        if (lowRow == highRow && lowCol == highCol) {
            Session* s = getSession(sessionID);
            if (!s) return false;
            return s->seats[lowRow][lowCol].status == 'O';
        }

        int midRow = (lowRow + highRow) / 2;
        int midCol = (lowCol + highCol) / 2;

        return dcCariSeatKosong(sessionID, lowRow, midRow, lowCol, midCol) ||
               dcCariSeatKosong(sessionID, lowRow, midRow, midCol + 1, highCol) ||
               dcCariSeatKosong(sessionID, midRow + 1, highRow, lowCol, midCol) ||
               dcCariSeatKosong(sessionID, midRow + 1, highRow, midCol + 1, highCol);
    }
};

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
