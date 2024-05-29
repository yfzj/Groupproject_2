#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <map>

using namespace std;

// Structure definitions
struct ParkingSpot {
    string type;
    bool isOccupied;
    string vehicleType;
    string plateNumber;
    time_t startTime;
    int entrance;
};

struct Customer {
    string plateNumber;
    time_t startTime;
    time_t endTime;
    string parkingType;
    string vehicleType;
    int entrance;
    int exit;
    double payment;
};

// Global variables
map<string, vector<ParkingSpot>> parkingLots;
map<string, Customer> customers;
string adminPassword;
double hourlyRates[3] = { 2.0, 3.0, 1.5 }; // Compact, Handicapped, Motorcycle
double dailyMaxRate = 50.0;

// Function declarations
void initializeSystem();
void adminLogin();
void customerLogin();
void displayParkingStatus();
void addParkingSpot();
void modifyParkingSpot();
void deleteParkingSpot();
void setHourlyRate();
void setDailyMaxRate();
void searchAvailableSpots();
void rentParkingSpot();
void settleParkingFee();
void saveData();
void loadData();

int main() {
    initializeSystem();
    int choice;
    do {
        cout << "Welcome to the Parking Management System\n";
        cout << "1. Admin Login\n";
        cout << "2. Customer Login\n";
        cout << "0. Exit\n";
        cout << "Please choose: ";
        cin >> choice;
        switch (choice) {
        case 1: adminLogin(); break;
        case 2: customerLogin(); break;
        }
    } while (choice != 0);
    saveData();
    return 0;
}

void initializeSystem() {
    // Set admin password
    cout << "Please set the admin password: ";
    cin >> adminPassword;

    // Load parking lot data from file
    loadData();
}

void adminLogin() {
    string password;
    cout << "Please enter the admin password: ";
    cin >> password;
    if (password == adminPassword) {
        int choice;
        do {
            cout << "Admin System\n";
            cout << "1. Browse Parking Information\n";
            cout << "2. Add Parking Spot\n";
            cout << "3. Modify Parking Spot\n";
            cout << "4. Delete Parking Spot\n";
            cout << "5. Set Hourly Rates\n";
            cout << "6. Set Daily Max Rate\n";
            cout << "7. Search Available Spots\n";
            cout << "0. Exit\n";
            cout << "Please choose: ";
            cin >> choice;
            switch (choice) {
            case 1: displayParkingStatus(); break;
            case 2: addParkingSpot(); break;
            case 3: modifyParkingSpot(); break;
            case 4: deleteParkingSpot(); break;
            case 5: setHourlyRate(); break;
            case 6: setDailyMaxRate(); break;
            case 7: searchAvailableSpots(); break;
            }
        } while (choice != 0);
    }
    else {
        cout << "Incorrect password\n";
    }
}

void customerLogin() {
    string plateNumber;
    cout << "Please enter your plate number: ";
    cin >> plateNumber;

    if (customers.find(plateNumber) == customers.end()) {
        // New customer
        Customer newCustomer;
        newCustomer.plateNumber = plateNumber;
        customers[plateNumber] = newCustomer;
    }

    int choice;
    do {
        cout << "Customer System\n";
        cout << "1. Search Available Spots\n";
        cout << "2. Rent Parking Spot\n";
        cout << "3. Settle Parking Fee\n";
        cout << "0. Exit\n";
        cout << "Please choose: ";
        cin >> choice;
        switch (choice) {
        case 1: searchAvailableSpots(); break;
        case 2: rentParkingSpot(); break;
        case 3: settleParkingFee(); break;
        }
    } while (choice != 0);
}

void displayParkingStatus() {
    for (const auto& floor : parkingLots) {
        cout << "Floor: " << floor.first << "\n";
        for (const auto& spot : floor.second) {
            cout << "Type: " << spot.type << ", Occupied: " << (spot.isOccupied ? "Yes" : "No") << "\n";
        }
    }
}

void addParkingSpot() {
    string floor;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;

    ParkingSpot newSpot;
    cout << "Enter spot type (Compact, Handicapped, Motorcycle): ";
    cin >> newSpot.type;
    newSpot.isOccupied = false;

    parkingLots[floor].push_back(newSpot);
    cout << "Parking spot added successfully\n";
}

void modifyParkingSpot() {
    string floor, type, newType;
    int index;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;
    cout << "Enter index of the spot to modify: ";
    cin >> index;

    if (parkingLots.find(floor) != parkingLots.end() && index >= 0 && index < parkingLots[floor].size()) {
        cout << "Current Type: " << parkingLots[floor][index].type << ", Is Occupied: " << (parkingLots[floor][index].isOccupied ? "Yes" : "No") << "\n";
        cout << "Enter new type (Compact, Handicapped, Motorcycle): ";
        cin >> newType;

        // Update the type
        parkingLots[floor][index].type = newType;
        cout << "Parking spot modified successfully\n";
    }
    else {
        cout << "Invalid floor or index\n";
    }
}

void deleteParkingSpot() {
    string floor;
    int index;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;
    cout << "Enter index of the spot to delete: ";
    cin >> index;

    if (parkingLots.find(floor) != parkingLots.end() && index >= 0 && index < parkingLots[floor].size()) {
        parkingLots[floor].erase(parkingLots[floor].begin() + index);
        cout << "Parking spot deleted successfully\n";
    }
    else {
        cout << "Invalid floor or index\n";
    }
}


void setHourlyRate() {
    cout << "Enter hourly rate for Compact spots: ";
    cin >> hourlyRates[0];
    cout << "Enter hourly rate for Handicapped spots: ";
    cin >> hourlyRates[1];
    cout << "Enter hourly rate for Motorcycle spots: ";
    cin >> hourlyRates[2];
}

void setDailyMaxRate() {
    cout << "Enter daily maximum rate: ";
    cin >> dailyMaxRate;
}

void searchAvailableSpots() {
    string vehicleType;
    cout << "Enter vehicle type (Car, Van, Truck, Motorcycle): ";
    cin >> vehicleType;

    for (const auto& floor : parkingLots) {
        cout << "Floor: " << floor.first << "\n";
        for (const auto& spot : floor.second) {
            if (!spot.isOccupied && spot.vehicleType == vehicleType) {
                cout << "Type: " << spot.type << ", Available\n";
            }
        }
    }
}

void rentParkingSpot() {
    string plateNumber;
    cout << "Enter your plate number: ";
    cin >> plateNumber;

    string floor, type;
    int entrance;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;
    cout << "Enter spot type (Compact, Handicapped, Motorcycle): ";
    cin >> type;
    cout << "Enter entrance (1 or 2): ";
    cin >> entrance;

    for (auto& spot : parkingLots[floor]) {
        if (!spot.isOccupied && spot.type == type) {
            spot.isOccupied = true;
            spot.vehicleType = type;
            spot.plateNumber = plateNumber;
            spot.startTime = time(nullptr);
            spot.entrance = entrance;
            customers[plateNumber].startTime = spot.startTime;
            customers[plateNumber].entrance = entrance;
            cout << "Parking spot rented successfully\n";
            return;
        }
    }
    cout << "No available spots of the requested type\n";
}

void settleParkingFee() {
    string plateNumber;
    cout << "Enter your plate number: ";
    cin >> plateNumber;

    if (customers.find(plateNumber) == customers.end()) {
        cout << "No such customer\n";
        return;
    }

    Customer& customer = customers[plateNumber];
    customer.endTime = time(nullptr);
    double totalHours = difftime(customer.endTime, customer.startTime) / 3600.0;
    double rate = hourlyRates[0]; // Default to Compact rate for now

    // Adjust rate based on parking type
    if (customer.parkingType == "Handicapped") {
        rate = hourlyRates[1];
    }
    else if (customer.parkingType == "Motorcycle") {
        rate = hourlyRates[2];
    }

    customer.payment = totalHours * rate;
    if (customer.payment > dailyMaxRate) {
        customer.payment = dailyMaxRate;
    }

    cout << "Total hours parked: " << totalHours << "\n";
    cout << "Total payment due: $" << customer.payment << "\n";

    // Reset parking spot and customer info
    for (auto& floor : parkingLots) {
        for (auto& spot : floor.second) {
            if (spot.plateNumber == plateNumber) {
                spot.isOccupied = false;
                spot.vehicleType = "";
                spot.plateNumber = "";
                spot.startTime = 0;
                break;
            }
        }
    }

    customers.erase(plateNumber);
    cout << "Payment settled and receipt printed\n";
}

void saveData() {
    ofstream outFile("parkingLots.dat");
    for (const auto& floor : parkingLots) {
        outFile << floor.first << "\n";
        for (const auto& spot : floor.second) {
            outFile << spot.type << " " << spot.isOccupied << " " << spot.vehicleType << " " << spot.plateNumber << " " << spot.startTime << " " << spot.entrance << "\n";
        }
    }
    outFile.close();

    outFile.open("customers.dat");
    for (const auto& customer : customers) {
        outFile << customer.first << " " << customer.second.startTime << " " << customer.second.endTime << " " << customer.second.parkingType << " " << customer.second.vehicleType << " " << customer.second.entrance << " " << customer.second.exit << " " << customer.second.payment << "\n";
    }
    outFile.close();
}

void loadData() {
    ifstream inFile("parkingLots.dat");
    if (inFile) {
        string floor, type, vehicleType, plateNumber;
        bool isOccupied;
        time_t startTime;
        int entrance;

        while (inFile >> floor) {
            while (inFile >> type >> isOccupied >> vehicleType >> plateNumber >> startTime >> entrance) {
                ParkingSpot spot = { type, isOccupied, vehicleType, plateNumber, startTime, entrance };
                parkingLots[floor].push_back(spot);
            }
        }
        inFile.close();
    }

    inFile.open("customers.dat");
    if (inFile) {
        string plateNumber, parkingType, vehicleType;
        time_t startTime, endTime;
        int entrance, exit;
        double payment;

        while (inFile >> plateNumber >> startTime >> endTime >> parkingType >> vehicleType >> entrance >> exit >> payment) {
            Customer customer = { plateNumber, startTime, endTime, parkingType, vehicleType, entrance, exit, payment };
            customers[plateNumber] = customer;
        }
        inFile.close();
    }
}
