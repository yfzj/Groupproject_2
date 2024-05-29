#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <set>
#include <cstdlib>  // For system("clear")

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
map<string, set<string>> parkingTypeToVehicleTypes;
map<string, map<string, double>> hourlyRates; // Nested map for parkingType -> vehicleType -> rate
string adminPassword;
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
void modifyParkingTypeVehicleTypes();
void clearScreen();

int main() {
    initializeSystem();
    int choice;
    do {
        clearScreen();
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

    // Initialize parking types and vehicle types
    parkingTypeToVehicleTypes["Compact"] = { "Car", "Van" };
    parkingTypeToVehicleTypes["Handicapped"] = { "Truck", "Otto" };
    parkingTypeToVehicleTypes["Motorcycle"] = { "Motorcycle" };

    // Initialize hourly rates
    hourlyRates["Compact"]["Car"] = 2.0;
    hourlyRates["Compact"]["Van"] = 2.5;
    hourlyRates["Handicapped"]["Truck"] = 3.0;
    hourlyRates["Handicapped"]["Otto"] = 3.5;
    hourlyRates["Motorcycle"]["Motorcycle"] = 1.5;

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
            clearScreen();
            cout << "Admin System\n";
            cout << "1. Browse Parking Information\n";
            cout << "2. Add Parking Spot\n";
            cout << "3. Modify Parking Spot\n";
            cout << "4. Delete Parking Spot\n";
            cout << "5. Set Hourly Rates\n";
            cout << "6. Set Daily Max Rate\n";
            cout << "7. Modify Parking Types and Vehicle Types\n";
            cout << "8. Search Available Spots\n";
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
            case 7: modifyParkingTypeVehicleTypes(); break;
            case 8: searchAvailableSpots(); break;
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
        clearScreen();
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
    clearScreen();
    for (const auto& floor : parkingLots) {
        cout << "Floor: " << floor.first << "\n";
        for (size_t i = 0; i < floor.second.size(); ++i) {
            const auto& spot = floor.second[i];
            cout << "Index: " << i << ", Type: " << spot.type << ", Occupied: " << (spot.isOccupied ? "Yes" : "No") << "\n";
        }
    }
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void addParkingSpot() {
    string floor;
    int count;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;
    cout << "Enter number of spots to add: ";
    cin >> count;

    ParkingSpot newSpot;
    cout << "Enter spot type (Compact, Handicapped, Motorcycle): ";
    cin >> newSpot.type;
    if (parkingTypeToVehicleTypes.find(newSpot.type) == parkingTypeToVehicleTypes.end()) {
        cout << "Invalid parking type\n";
        return;
    }
    newSpot.isOccupied = false;

    for (int i = 0; i < count; ++i) {
        parkingLots[floor].push_back(newSpot);
    }
    cout << "Parking spots added successfully\n";
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void modifyParkingSpot() {
    string floor, newType, newVehicleType;
    int index;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;
    cout << "Enter index of the spot to modify: ";
    cin >> index;

    if (parkingLots.find(floor) != parkingLots.end() && index >= 0 && index < parkingLots[floor].size()) {
        auto& spot = parkingLots[floor][index];
        cout << "Current Type: " << spot.type << ", Is Occupied: " << (spot.isOccupied ? "Yes" : "No") << "\n";
        cout << "Enter new type (Compact, Handicapped, Motorcycle): ";
        cin >> newType;

        if (parkingTypeToVehicleTypes.find(newType) != parkingTypeToVehicleTypes.end()) {
            spot.type = newType;
            cout << "Enter new vehicle type: ";
            cin >> newVehicleType;
            if (parkingTypeToVehicleTypes[newType].find(newVehicleType) != parkingTypeToVehicleTypes[newType].end()) {
                spot.vehicleType = newVehicleType;
                cout << "Parking spot modified successfully\n";
            }
            else {
                cout << "Invalid vehicle type for the selected parking type\n";
            }
        }
        else {
            cout << "Invalid parking type\n";
        }
    }
    else {
        cout << "Invalid floor or index\n";
    }
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
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
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void setHourlyRate() {
    string parkingType, vehicleType;
    double rate;
    cout << "Enter parking type (Compact, Handicapped, Motorcycle): ";
    cin >> parkingType;
    cout << "Enter vehicle type: ";
    cin >> vehicleType;
    cout << "Enter hourly rate: ";
    cin >> rate;

    if (parkingTypeToVehicleTypes.find(parkingType) != parkingTypeToVehicleTypes.end() &&
        parkingTypeToVehicleTypes[parkingType].find(vehicleType) != parkingTypeToVehicleTypes[parkingType].end()) {
        hourlyRates[parkingType][vehicleType] = rate;
        cout << "Hourly rate set successfully\n";
    }
    else {
        cout << "Invalid parking type or vehicle type\n";
    }
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void setDailyMaxRate() {
    cout << "Enter daily maximum rate: ";
    cin >> dailyMaxRate;
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void searchAvailableSpots() {
    clearScreen();
    string vehicleType;
    cout << "Enter vehicle type (Car, Van, Truck, Motorcycle): ";
    cin >> vehicleType;

    for (const auto& floor : parkingLots) {
        cout << "Floor: " << floor.first << "\n";
        for (size_t i = 0; i < floor.second.size(); ++i) {
            const auto& spot = floor.second[i];
            if (!spot.isOccupied && parkingTypeToVehicleTypes[spot.type].find(vehicleType) != parkingTypeToVehicleTypes[spot.type].end()) {
                cout << "Index: " << i << ", Type: " << spot.type << ", Available\n";
            }
        }
    }
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void rentParkingSpot() {
    clearScreen();
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

    if (parkingTypeToVehicleTypes.find(type) == parkingTypeToVehicleTypes.end()) {
        cout << "Invalid parking type\n";
        return;
    }

    string vehicleType;
    cout << "Enter vehicle type: ";
    cin >> vehicleType;

    if (parkingTypeToVehicleTypes[type].find(vehicleType) == parkingTypeToVehicleTypes[type].end()) {
        cout << "Invalid vehicle type for the selected parking type\n";
        return;
    }

    for (auto& spot : parkingLots[floor]) {
        if (!spot.isOccupied && spot.type == type) {
            spot.isOccupied = true;
            spot.vehicleType = vehicleType;
            spot.plateNumber = plateNumber;
            spot.startTime = time(nullptr);
            spot.entrance = entrance;
            customers[plateNumber].startTime = spot.startTime;
            customers[plateNumber].entrance = entrance;
            customers[plateNumber].parkingType = type;
            customers[plateNumber].vehicleType = vehicleType;
            cout << "Parking spot rented successfully\n";
            cout << "Press any key to continue...";
            cin.ignore();
            cin.get();
            return;
        }
    }
    cout << "No available spots of the requested type\n";
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void settleParkingFee() {
    clearScreen();
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
    double rate = hourlyRates[customer.parkingType][customer.vehicleType];
    customer.payment = totalHours * rate;

    // Increase hourly rate by 20% for every 6 hours parked
    int sixHourIntervals = static_cast<int>(totalHours) / 6;
    customer.payment *= (1 + 0.2 * sixHourIntervals);

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
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void modifyParkingTypeVehicleTypes() {
    clearScreen();
    string parkingType, vehicleType;
    cout << "Enter parking type to modify (Compact, Handicapped, Motorcycle): ";
    cin >> parkingType;

    if (parkingTypeToVehicleTypes.find(parkingType) == parkingTypeToVehicleTypes.end()) {
        cout << "Invalid parking type\n";
        return;
    }

    cout << "Enter vehicle type to add or remove: ";
    cin >> vehicleType;
    char choice;
    cout << "Add or remove (a/r): ";
    cin >> choice;

    if (choice == 'a') {
        parkingTypeToVehicleTypes[parkingType].insert(vehicleType);
        cout << "Vehicle type added to parking type\n";
    }
    else if (choice == 'r') {
        parkingTypeToVehicleTypes[parkingType].erase(vehicleType);
        cout << "Vehicle type removed from parking type\n";
    }
    else {
        cout << "Invalid choice\n";
    }
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
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

    outFile.open("parkingTypeToVehicleTypes.dat");
    for (const auto& type : parkingTypeToVehicleTypes) {
        outFile << type.first;
        for (const auto& vehicle : type.second) {
            outFile << " " << vehicle;
        }
        outFile << "\n";
    }
    outFile.close();

    outFile.open("hourlyRates.dat");
    for (const auto& type : hourlyRates) {
        for (const auto& rate : type.second) {
            outFile << type.first << " " << rate.first << " " << rate.second << "\n";
        }
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
            vector<ParkingSpot> spots;
            while (inFile >> type >> isOccupied >> vehicleType >> plateNumber >> startTime >> entrance) {
                ParkingSpot spot = { type, isOccupied, vehicleType, plateNumber, startTime, entrance };
                spots.push_back(spot);
                if (inFile.peek() == '\n') break;
            }
            parkingLots[floor] = spots;
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

    inFile.open("parkingTypeToVehicleTypes.dat");
    if (inFile) {
        string parkingType, vehicleType;
        while (inFile >> parkingType) {
            set<string> vehicleTypes;
            while (inFile >> vehicleType) {
                if (vehicleType == "\n") break;
                vehicleTypes.insert(vehicleType);
            }
            parkingTypeToVehicleTypes[parkingType] = vehicleTypes;
        }
        inFile.close();
    }

    inFile.open("hourlyRates.dat");
    if (inFile) {
        string parkingType, vehicleType;
        double rate;
        while (inFile >> parkingType >> vehicleType >> rate) {
            hourlyRates[parkingType][vehicleType] = rate;
        }
        inFile.close();
    }
}

void clearScreen() {
    system("cls");
}

