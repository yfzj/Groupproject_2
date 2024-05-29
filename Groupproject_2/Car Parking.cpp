#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

// Structure definitions
struct ParkingSpot {
    string id;
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
map<string, map<string, double>> hourlyRates;
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
string generateParkingSpotId(const string& floor, int index);
void displayVisualParkingStatus(const string& floor);

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
    return 0;
}

void initializeSystem() {
    loadData();

    if (adminPassword.empty()) {
        cout << "Please set the admin password: ";
        cin >> adminPassword;
        saveData();
    }
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
        displayVisualParkingStatus(floor.first);
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

    // 显示当前的停车位类型供用户选择
    cout << "Available parking types: ";
    for (const auto& type : parkingTypeToVehicleTypes) {
        cout << type.first << " ";
    }
    cout << "\nEnter spot type: ";

    ParkingSpot newSpot;
    cin >> newSpot.type;
    if (parkingTypeToVehicleTypes.find(newSpot.type) == parkingTypeToVehicleTypes.end()) {
        cout << "Invalid parking type\n";
        return;
    }
    newSpot.isOccupied = false;

    auto& spots = parkingLots[floor];
    int currentSize = spots.size();
    for (int i = 0; i < count; ++i) {
        auto it = find_if(spots.begin(), spots.end(), [](const ParkingSpot& spot) {
            return spot.type.empty();
            });
        if (it != spots.end()) {
            it->type = newSpot.type;
            it->isOccupied = newSpot.isOccupied;
            it->vehicleType = newSpot.vehicleType;
            it->plateNumber = newSpot.plateNumber;
            it->startTime = newSpot.startTime;
            it->entrance = newSpot.entrance;
            it->id = generateParkingSpotId(floor, distance(spots.begin(), it));
        }
        else {
            newSpot.id = generateParkingSpotId(floor, currentSize + i);
            spots.push_back(newSpot);
        }
    }
    saveData();
    cout << "Parking spots added successfully\n";
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void modifyParkingSpot() {
    string floor, newType, newVehicleType;
    string spotId;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;
    cout << "Enter ID of the spot to modify:(e.g., B1_1,B1_2) ";
    cin >> spotId;

    if (parkingLots.find(floor) != parkingLots.end()) {
        auto& spots = parkingLots[floor];
        auto it = find_if(spots.begin(), spots.end(), [&spotId](const ParkingSpot& spot) {
            return spot.id == spotId;
            });

        if (it != spots.end()) {
            auto& spot = *it;
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
                    saveData();
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
            cout << "Invalid spot ID\n";
        }
    }
    else {
        cout << "Invalid floor\n";
    }
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void deleteParkingSpot() {
    string floor;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;

    if (parkingLots.find(floor) != parkingLots.end()) {
        auto& spots = parkingLots[floor];

        cout << "Available spots on " << floor << ":\n";
        for (const auto& spot : spots) {
            cout << spot.id << " ";
        }
        cout << "\n";

        cout << "Enter IDs of the spots to delete (separated by spaces): ";
        cin.ignore();
        string line;
        getline(cin, line);
        stringstream ss(line);
        string spotId;

        vector<string> idsToDelete;
        while (ss >> spotId) {
            idsToDelete.push_back(spotId);
        }

        for (const string& id : idsToDelete) {
            auto it = find_if(spots.begin(), spots.end(), [&id](const ParkingSpot& spot) {
                return spot.id == id;
                });

            if (it != spots.end()) {
                it->type.clear();
                cout << "Parking spot " << id << " deleted successfully\n";
            }
            else {
                cout << "Invalid spot ID: " << id << "\n";
            }
        }
        saveData();
    }
    else {
        cout << "Invalid floor\n";
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
        saveData();
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
    saveData();
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
        for (const auto& spot : floor.second) {
            if (!spot.isOccupied && parkingTypeToVehicleTypes[spot.type].find(vehicleType) != parkingTypeToVehicleTypes[spot.type].end()) {
                cout << "ID: " << spot.id << ", Type: " << spot.type << ", Available\n";
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
    string spotId;
    int entrance;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;
    cout << "Enter spot ID: ";
    cin >> spotId;
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

    auto& spots = parkingLots[floor];
    auto it = find_if(spots.begin(), spots.end(), [&spotId](const ParkingSpot& spot) {
        return spot.id == spotId;
        });

    if (it != spots.end() && !it->isOccupied && it->type == type) {
        it->isOccupied = true;
        it->vehicleType = vehicleType;
        it->plateNumber = plateNumber;
        it->startTime = time(nullptr);
        it->entrance = entrance;
        customers[plateNumber].startTime = it->startTime;
        customers[plateNumber].entrance = entrance;
        customers[plateNumber].parkingType = type;
        customers[plateNumber].vehicleType = vehicleType;
        saveData();
        cout << "Parking spot rented successfully\n";
    }
    else {
        cout << "No available spots of the requested type\n";
    }
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

    int sixHourIntervals = static_cast<int>(totalHours) / 6;
    customer.payment *= (1 + 0.2 * sixHourIntervals);

    if (customer.payment > dailyMaxRate) {
        customer.payment = dailyMaxRate;
    }

    cout << "Total hours parked: " << totalHours << "\n";
    cout << "Total payment due: $" << customer.payment << "\n";

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
    saveData();
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
    saveData();
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void saveData() {
    ofstream outFile("adminPassword.dat");
    if (outFile) {
        outFile << adminPassword;
        outFile.close();
    }

    outFile.open("parkingLots.dat");
    if (outFile) {
        for (const auto& floor : parkingLots) {
            outFile << floor.first << "\n";
            for (const auto& spot : floor.second) {
                outFile << spot.id << " " << spot.type << " " << spot.isOccupied << " "
                    << spot.vehicleType << " " << spot.plateNumber << " "
                    << spot.startTime << " " << spot.entrance << "\n";
            }
            outFile << "#\n"; // 标记楼层结束
        }
        outFile.close();
    }

    outFile.open("customers.dat");
    if (outFile) {
        for (const auto& customer : customers) {
            outFile << customer.first << " " << customer.second.startTime << " "
                << customer.second.endTime << " " << customer.second.parkingType << " "
                << customer.second.vehicleType << " " << customer.second.entrance << " "
                << customer.second.exit << " " << customer.second.payment << "\n";
        }
        outFile.close();
    }

    outFile.open("parkingTypeToVehicleTypes.dat");
    if (outFile) {
        for (const auto& type : parkingTypeToVehicleTypes) {
            outFile << type.first;
            for (const auto& vehicle : type.second) {
                outFile << " " << vehicle;
            }
            outFile << "\n";
        }
        outFile.close();
    }

    outFile.open("hourlyRates.dat");
    if (outFile) {
        for (const auto& type : hourlyRates) {
            for (const auto& rate : type.second) {
                outFile << type.first << " " << rate.first << " " << rate.second << "\n";
            }
        }
        outFile.close();
    }
}

void loadData() {
    ifstream inFile;

    // Load admin password
    inFile.open("adminPassword.dat");
    if (inFile) {
        inFile >> adminPassword;
        inFile.close();
    }
    else {
        adminPassword = "";
    }

    // Load parking lots
    inFile.open("parkingLots.dat");
    if (inFile) {
        string floor;
        while (getline(inFile, floor)) {
            vector<ParkingSpot> spots;
            string line;
            while (getline(inFile, line)) {
                if (line == "#") break; // 结束当前楼层的读取
                istringstream iss(line);
                ParkingSpot spot;
                iss >> spot.id >> spot.type >> spot.isOccupied >> spot.vehicleType
                    >> spot.plateNumber >> spot.startTime >> spot.entrance;
                spots.push_back(spot);
            }
            parkingLots[floor] = spots;
        }
        inFile.close();
    }

    // Load customers
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

    // Load parkingTypeToVehicleTypes
    inFile.open("parkingTypeToVehicleTypes.dat");
    if (inFile) {
        string parkingType, vehicleType;
        while (inFile >> parkingType) {
            set<string> vehicleTypes;
            while (inFile.peek() != '\n' && inFile >> vehicleType) {
                vehicleTypes.insert(vehicleType);
            }
            parkingTypeToVehicleTypes[parkingType] = vehicleTypes;
            inFile.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略行末的换行符
        }
        inFile.close();
    }
    else {
        // 初始化默认值
        parkingTypeToVehicleTypes["Compact"] = { "Car", "Van" };
        parkingTypeToVehicleTypes["Handicapped"] = { "Truck", "Otto" };
        parkingTypeToVehicleTypes["Motorcycle"] = { "Motorcycle" };
    }

    // Load hourly rates
    inFile.open("hourlyRates.dat");
    if (inFile) {
        string parkingType, vehicleType;
        double rate;
        while (inFile >> parkingType >> vehicleType >> rate) {
            hourlyRates[parkingType][vehicleType] = rate;
        }
        inFile.close();
    }
    else {
        // 初始化默认值
        hourlyRates["Compact"]["Car"] = 2.0;
        hourlyRates["Compact"]["Van"] = 2.5;
        hourlyRates["Handicapped"]["Truck"] = 3.0;
        hourlyRates["Handicapped"]["Otto"] = 3.5;
        hourlyRates["Motorcycle"]["Motorcycle"] = 1.5;
    }
}

void clearScreen() {
    system("cls");
}

string generateParkingSpotId(const string& floor, int index) {
    return floor + "_" + to_string(index + 1);
}

void displayVisualParkingStatus(const string& floor) {
    cout << "Floor: " << floor << "\n";
    const auto& spots = parkingLots[floor];
    const int columnWidth = 20;

    for (size_t i = 0; i < spots.size(); ++i) {
        if (i % 5 == 0 && i != 0) cout << "\n";
        const auto& spot = spots[i];
        string spotStatus = spot.isOccupied ? "[X]" : "[ ]";
        cout << left << setw(columnWidth) << (spotStatus + spot.id + "(" + spot.type + ")");
    }
    cout << "\n";
}
