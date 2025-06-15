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

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void login(Bioskop& bio, User*& currentUser) {
    cout << "=== LOGIN ===\n";
    string username, password;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;

    User* u = bio.findUser(username);
    if (u && u->checkPassword(password)) {
        cout << "Login berhasil, selamat datang " << username << "!\n";
        currentUser = u;
    } else {
        cout << "Username atau password salah!\n";
    }
}

void registerUser(Bioskop& bio) {
    cout << "=== REGISTER ===\n";
    string username, password, passConfirm;
    cout << "Username: "; cin >> username;

    if (bio.findUser(username)) {
        cout << "Username sudah digunakan.\n";
        return;
    }

    cout << "Password: "; cin >> password;
    cout << "Konfirmasi Password: "; cin >> passConfirm;

    if (password != passConfirm) {
        cout << "Password tidak cocok.\n";
        return;
    }

    bio.addUser(username, password);
    cout << "Registrasi berhasil, silahkan login.\n";
}

void bookingMenu(Bioskop& bio, User* currentUser) {
    cout << "=== Booking Tempat Duduk ===\n";
    bio.showSessions();

    int sesi;
    cout << "Pilih sesi (1-3): ";
    cin >> sesi;
    if (sesi < 1 || sesi > 3) {
        cout << "Sesi tidak valid!\n";
        return;
    }

    bio.showSeats(sesi);

    cout << "Masukkan kode kursi (misal A1): ";
    string seat;
    cin >> seat;

    if (!bio.validSeat(seat)) {
        cout << "Kode kursi tidak valid!\n";
        return;
    }

    if (!bio.seatAvailable(sesi, seat)) {
        cout << "Kursi sudah dipesan!\n";
        return;
    }

    if (currentUser->hasBooked(seat, sesi)) {
        cout << "Kamu sudah booking kursi ini di sesi ini!\n";
        return;
    }

    bool booked = bio.bookSeat(sesi, seat);
    if (booked) {
        currentUser->addBooking(seat, sesi);
        cout << "Booking berhasil untuk kursi " << seat << " di sesi " << sesi << ".\n";
    } else {
        cout << "Booking gagal.\n";
    }
}

void cancelBookingMenu(Bioskop& bio, User* currentUser) {
    cout << "=== Batal Booking ===\n";
    currentUser->showBookings();

    cout << "Masukkan kode kursi yang ingin dibatalkan (misal A1): ";
    string seat;
    cin >> seat;

    cout << "Masukkan sesi kursi yang ingin dibatalkan: ";
    int sesi;
    cin >> sesi;

    if (!bio.validSeat(seat)) {
        cout << "Kode kursi tidak valid!\n";
        return;
    }

    if (!currentUser->hasBooked(seat, sesi)) {
        cout << "Kamu belum booking kursi ini di sesi ini.\n";
        return;
    }

    bool cancelSeatSuccess = bio.cancelSeat(sesi, seat);
    bool cancelBookingSuccess = currentUser->cancelBooking(seat, sesi);

    if (cancelSeatSuccess && cancelBookingSuccess) {
        cout << "Booking kursi " << seat << " di sesi " << sesi << " berhasil dibatalkan.\n";
    } else {
        cout << "Gagal membatalkan booking.\n";
    }
}

void showMainMenu() {
    cout << "\n=== APLIKASI BOOKING BIOSKOP ===\n";
    cout << "1. Login\n";
    cout << "2. Register\n";
    cout << "3. Keluar\n";
    cout << "Pilih menu: ";
}

void showUserMenu(string username) {
    cout << "\n=== Menu User: " << username << " ===\n";
    cout << "1. Booking kursi\n";
    cout << "2. Batal booking\n";
    cout << "3. Lihat booking saya\n";
    cout << "4. Cari kursi kosong (divide & conquer)\n";
    cout << "5. Logout\n";
    cout << "Pilih menu: ";
}

void cariKursiKosongMenu(Bioskop& bio) {
    cout << "Pilih sesi untuk cari kursi kosong: ";
    int sesi;
    cin >> sesi;
    if (sesi < 1 || sesi > 3) {
        cout << "Sesi tidak valid!\n";
        return;
    }
    bool found = bio.dcCariSeatKosong(sesi, 0, 4, 0, 5);
    if (found)
        cout << "Ada kursi kosong di sesi " << sesi << "!\n";
    else
        cout << "Tidak ada kursi kosong di sesi " << sesi << ".\n";
}

int main() {
    Bioskop bio;
    User* currentUser = nullptr;
    bool running = true;

    while (running) {
        if (!currentUser) {
            showMainMenu();
            int choice;
            cin >> choice;
            clearInput();

            switch (choice) {
                case 1:
                    login(bio, currentUser);
                    break;
                case 2:
                    registerUser(bio);
                    break;
                case 3:
                    cout << "Terima kasih sudah menggunakan aplikasi ini.\n";
                    running = false;
                    break;
                default:
                    cout << "Pilihan tidak valid.\n";
                    break;
            }
        } else {
            showUserMenu(currentUser->getUsername());
            int choice;
            cin >> choice;
            clearInput();

            switch (choice) {
                case 1:
                    bookingMenu(bio, currentUser);
                    break;
                case 2:
                    cancelBookingMenu(bio, currentUser);
                    break;
                case 3:
                    currentUser->showBookings();
                    break;
                case 4:
                    cariKursiKosongMenu(bio);
                    break;
                case 5:
                    cout << "Logout berhasil.\n";
                    currentUser = nullptr;
                    break;
                default:
                    cout << "Pilihan tidak valid.\n";
                    break;
            }
        }
    }
    return 0;
}
