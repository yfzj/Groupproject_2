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
    string floor, newType;
    cout << "Enter floor you want modify (e.g., B1, B2): ";
    cin >> floor;

    if (parkingLots.find(floor) != parkingLots.end()) {
        auto& spots = parkingLots[floor];

        // Display current parking spots information on the selected floor
        map<string, vector<string>> typeToSpots;
        for (const auto& spot : spots) {
            if (!spot.type.empty()) {
                typeToSpots[spot.type].push_back(spot.id);
            }
        }

        cout << "Current parking spots on " << floor << ":\n";
        for (const auto& entry : typeToSpots) {
            cout << entry.first << ": ";
            for (size_t i = 0; i < entry.second.size(); ++i) {
                if (i > 0 && stoi(entry.second[i].substr(entry.second[i].find('_') + 1)) != stoi(entry.second[i - 1].substr(entry.second[i - 1].find('_') + 1)) + 1) {
                    cout << ", " << entry.second[i];
                }
                else if (i > 0 && (i == entry.second.size() - 1 || stoi(entry.second[i + 1].substr(entry.second[i + 1].find('_') + 1)) != stoi(entry.second[i].substr(entry.second[i].find('_') + 1)) + 1)) {
                    cout << " to " << entry.second[i];
                }
                else if (i == 0) {
                    cout << entry.second[i];
                }
            }
            cout << "\n";
        }

        cout << "Choose modification type:\n";
        cout << "1. Modify multiple individual spots\n";
        cout << "2. Modify a range of spots\n";
        int choice;
        cin >> choice;

        vector<string> idsToModify;

        if (choice == 1) {
            // Input for individual spot IDs to modify
            string spotIds;
            cout << "Enter IDs of the spots to modify (separated by spaces): \n";
            cout << "(Such as B1_1 B1_2 to modify the spots 1, 2 from B1)\n";
            cin.ignore(); // Ignore any leftover newline character
            getline(cin, spotIds);
            stringstream ss(spotIds);
            string spotId;
            while (ss >> spotId) {
                idsToModify.push_back(spotId);
            }
        }
        else if (choice == 2) {
            // Input for range of spot IDs to modify
            string startId, endId;
            cout << "Enter the start ID of the range to modify (e.g., B1_1): ";
            cin >> startId;
            cout << "Enter the end ID of the range to modify (e.g., B1_10): ";
            cin >> endId;

            int startIdx = stoi(startId.substr(startId.find('_') + 1));
            int endIdx = stoi(endId.substr(endId.find('_') + 1));

            for (int i = startIdx; i <= endIdx; ++i) {
                idsToModify.push_back(floor + "_" + to_string(i));
            }
        }
        else {
            cout << "Invalid choice\n";
            return;
        }

        // Show available parking types
        cout << "Available parking types: ";
        for (const auto& type : parkingTypeToVehicleTypes) {
            cout << type.first << " ";
        }
        cout << "\nEnter new spot type: ";
        cin >> newType;

        if (parkingTypeToVehicleTypes.find(newType) == parkingTypeToVehicleTypes.end()) {
            cout << "Invalid parking type\n";
            return;
        }

        // Modify the specified spots
        for (const string& id : idsToModify) {
            auto it = find_if(spots.begin(), spots.end(), [&id](const ParkingSpot& spot) {
                return spot.id == id;
                });

            if (it != spots.end()) {
                it->type = newType;
                cout << "Parking spot " << id << " modified successfully\n";
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

void deleteParkingSpot() {
    string floor;
    cout << "Enter floor you want to delete spots from (e.g., B1, B2): ";
    cin >> floor;

    if (parkingLots.find(floor) != parkingLots.end()) {
        auto& spots = parkingLots[floor];

        // Display current parking spots information on the selected floor
        map<string, vector<string>> typeToSpots;
        for (const auto& spot : spots) {
            if (!spot.type.empty()) {
                typeToSpots[spot.type].push_back(spot.id);
            }
        }

        cout << "Current parking spots on " << floor << ":\n";
        for (const auto& entry : typeToSpots) {
            cout << entry.first << ": ";
            for (size_t i = 0; i < entry.second.size(); ++i) {
                if (i > 0 && stoi(entry.second[i].substr(entry.second[i].find('_') + 1)) != stoi(entry.second[i - 1].substr(entry.second[i - 1].find('_') + 1)) + 1) {
                    cout << ", " << entry.second[i];
                }
                else if (i > 0 && (i == entry.second.size() - 1 || stoi(entry.second[i + 1].substr(entry.second[i + 1].find('_') + 1)) != stoi(entry.second[i].substr(entry.second[i].find('_') + 1)) + 1)) {
                    cout << " to " << entry.second[i];
                }
                else if (i == 0) {
                    cout << entry.second[i];
                }
            }
            cout << "\n";
        }

        cout << "Choose deletion type:\n";
        cout << "1. Delete multiple individual spots\n";
        cout << "2. Delete a range of spots\n";
        int choice;
        cin >> choice;

        vector<string> idsToDelete;

        if (choice == 1) {
            // Input for individual spot IDs to delete
            string spotIds;
            cout << "Enter IDs of the spots to delete (separated by spaces): \n";
            cout << "(Such as B1_1 B1_2 to delete the spots 1, 2 from B1)\n";
            cin.ignore(); // Ignore any leftover newline character
            getline(cin, spotIds);
            stringstream ss(spotIds);
            string spotId;
            while (ss >> spotId) {
                idsToDelete.push_back(spotId);
            }
        }
        else if (choice == 2) {
            // Input for range of spot IDs to delete
            string startId, endId;
            cout << "Enter the start ID of the range to delete (e.g., B1_1): ";
            cin >> startId;
            cout << "Enter the end ID of the range to delete (e.g., B1_10): ";
            cin >> endId;

            int startIdx = stoi(startId.substr(startId.find('_') + 1));
            int endIdx = stoi(endId.substr(endId.find('_') + 1));

            for (int i = startIdx; i <= endIdx; ++i) {
                idsToDelete.push_back(floor + "_" + to_string(i));
            }
        }
        else {
            cout << "Invalid choice\n";
            return;
        }

        // Delete the specified spots
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
    string parkingType;
    double rate;

    // Display available parking types from file
    cout << "Available parking types: ";
    for (const auto& type : parkingTypeToVehicleTypes) {
        cout << type.first << " ";
    }
    cout << "\nEnter parking type: ";
    cin >> parkingType;

    if (parkingTypeToVehicleTypes.find(parkingType) != parkingTypeToVehicleTypes.end()) {
        // Display current hourly rate for the selected parking type
        if (hourlyRates.find(parkingType) != hourlyRates.end()) {
            cout << "Current hourly rate for " << parkingType << ": $" << hourlyRates[parkingType]["Default"] << "\n";
        }
        else {
            cout << "No current hourly rate set for " << parkingType << "\n";
        }

        cout << "Enter new hourly rate: ";
        cin >> rate;

        hourlyRates[parkingType]["Default"] = rate;  // Use a default key since vehicle type is no longer relevant
        saveData();
        cout << "Hourly rate set successfully\n";
    }
    else {
        cout << "Invalid parking type\n";
    }

    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void setDailyMaxRate() {
    // Display current daily maximum rate
    cout << "Current daily maximum rate: $" << dailyMaxRate << "\n";

    cout << "Enter new daily maximum rate: ";
    cin >> dailyMaxRate;
    saveData();
    cout << "Daily maximum rate set successfully\n";

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
            outFile << "#\n"; // Mark end of floor spots
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
            outFile << type.first << " " << type.second.at("Default") << "\n"; // Save the rate associated with the parking type
        }
        outFile.close();
    }

    outFile.open("dailyMaxRate.dat");
    if (outFile) {
        outFile << dailyMaxRate << "\n";
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
                if (line == "#") break; // End of current floor
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
            inFile.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the rest of the line
        }
        inFile.close();
    }
    else {
        // Initialize default values if file doesn't exist
        parkingTypeToVehicleTypes["Compact"] = { "Car", "Van" };
        parkingTypeToVehicleTypes["Handicapped"] = { "Truck", "Otto" };
        parkingTypeToVehicleTypes["Motorcycle"] = { "Motorcycle" };
    }

    // Load hourly rates
    inFile.open("hourlyRates.dat");
    if (inFile) {
        string parkingType;
        double rate;
        while (inFile >> parkingType >> rate) {
            hourlyRates[parkingType]["Default"] = rate; // Load the rate associated with the parking type
        }
        inFile.close();
    }
    else {
        // Initialize default hourly rates if file doesn't exist
        hourlyRates["Compact"]["Default"] = 2.0;
        hourlyRates["Handicapped"]["Default"] = 3.0;
        hourlyRates["Motorcycle"]["Default"] = 1.5;
    }

    // Load daily maximum rate
    inFile.open("dailyMaxRate.dat");
    if (inFile) {
        inFile >> dailyMaxRate;
        inFile.close();
    }
    else {
        dailyMaxRate = 50.0; // Default daily maximum rate if file doesn't exist
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
