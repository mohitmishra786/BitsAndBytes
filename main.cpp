#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;

int main() {
    try {
        // Get database credentials from environment variables
        const char* db_name_1 = std::getenv("DB_NAME_1");
        const char* db_name_2 = std::getenv("DB_NAME_2");
        const char* db_user = std::getenv("DB_USER");
        const char* db_password = std::getenv("DB_PASSWORD");

        // Check if credentials are set
        if (!db_name_1 || !db_name_2 || !db_user || !db_password) {
            cerr << "Environment variables not set" << endl;
            return 1;
        }

        // Create connection strings
        string conn_string_1 = "dbname=" + string(db_name_1) +
                              " user=" + string(db_user) +
                              " password=" + string(db_password);

        string conn_string_2 = "dbname=" + string(db_name_2) +
                              " user=" + string(db_user) +
                              " password=" + string(db_password);

        // Connect to databases
        pqxx::connection conn1(conn_string_1);
        pqxx::connection conn2(conn_string_2);

        cout << "Connected to databases successfully." << endl;

        // Start transactions
        pqxx::work w1(conn1);
        pqxx::work w2(conn2);

        // Execute queries
        w1.exec("INSERT INTO users values ('4', 'Whatever')");
        pqxx::result users_result = w1.exec("SELECT id, name FROM users");

        pqxx::result grades_result = w2.exec("SELECT * FROM grades WHERE rollNumber='1'");

        cout << "Users found: " << users_result.size() << endl;
        for (const auto& row : users_result) {
            int id = row[0].as<int>();
            string name = row[1].as<string>();
            cout << "ID: " << id << ", Name: " << name << endl;
        }

        cout << "Grades found: " << grades_result.size() << endl;
        for (const auto& row : grades_result) {
            int id = row[0].as<int>();
            string subject = row[1].as<string>();
            int marks = row[2].as<int>();
            int rollNumber = row[3].as<int>();

            cout << "ID: " << id
                 << ", Subject: " << subject
                 << ", Marks: " << marks
                 << ", Roll Number: " << rollNumber << endl;
        }

        // Commit transactions
        w1.commit();
        w2.commit();

        cout << "Transactions committed successfully." << endl;

    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
