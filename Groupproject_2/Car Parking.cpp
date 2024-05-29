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
#include <thread>   
#include <chrono>  

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
string currentPlateNumber;
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
void clearParkingSpotOccupation();
void manageCustomerInformation();
void viewCustomerInformation();
void addCustomerInformation();
void deleteCustomerInformation();

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
            cout << "9. Clear Parking Spot Occupation\n";
            cout << "10. Manage Customer Information\n";
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
            case 9: clearParkingSpotOccupation(); break;
            case 10: manageCustomerInformation(); break;
            case 0: break;
            default: cout << "Invalid choice\n";
            }
        } while (choice != 0);
    }
    else {
        cout << "Incorrect password\n";
        cout << "Press any key to continue...";
        cin.ignore();
        cin.get();
    }
}


void customerLogin() {
    cout << "Please enter your plate number: ";
    cin >> currentPlateNumber;

    if (customers.find(currentPlateNumber) == customers.end()) {
        Customer newCustomer;
        newCustomer.plateNumber = currentPlateNumber;
        customers[currentPlateNumber] = newCustomer;
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
    cout << "Enter floor you want to modify (e.g., B1, B2): ";
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
                it->isOccupied = false;  // Set the spot to be available
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
                it->isOccupied = true; // Set the spot to be unavailable
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

void modifyParkingTypeVehicleTypes() {
    clearScreen();
    string parkingType, vehicleType;

    // Display available parking types and their associated vehicle types
    cout << "Current parking types and their associated vehicle types:\n";
    for (const auto& type : parkingTypeToVehicleTypes) {
        cout << type.first << ": ";
        for (const auto& vehicle : type.second) {
            cout << vehicle << " ";
        }
        cout << "\n";
    }

    cout << "Enter parking type to modify: ";
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

void clearParkingSpotOccupation() {
    string floor, spotId;
    cout << "Enter floor (e.g., B1, B2): ";
    cin >> floor;

    if (parkingLots.find(floor) != parkingLots.end()) {
        auto& spots = parkingLots[floor];

        // 显示当前被占用的停车位信息
        cout << "Currently occupied spots on " << floor << ":\n";
        for (const auto& spot : spots) {
            if (spot.isOccupied) {
                cout << "ID: " << spot.id << ", Vehicle Type: " << spot.vehicleType << ", Plate Number: " << spot.plateNumber << "\n";
            }
        }

        cout << "Choose clearing type:\n";
        cout << "1. Clear multiple individual spots\n";
        cout << "2. Clear a range of spots\n";
        int choice;
        cin >> choice;

        vector<string> idsToClear;

        if (choice == 1) {
            // Input for individual spot IDs to clear
            string spotIds;
            cout << "Enter IDs of the spots to clear (separated by spaces): \n";
            cout << "(Such as B1_1 B1_2 to clear the spots 1, 2 from B1)\n";
            cin.ignore(); // Ignore any leftover newline character
            getline(cin, spotIds);
            stringstream ss(spotIds);
            string spotId;
            while (ss >> spotId) {
                idsToClear.push_back(spotId);
            }
        }
        else if (choice == 2) {
            // Input for range of spot IDs to clear
            string startId, endId;
            cout << "Enter the start ID of the range to clear (e.g., B1_1): ";
            cin >> startId;
            cout << "Enter the end ID of the range to clear (e.g., B1_10): ";
            cin >> endId;

            int startIdx = stoi(startId.substr(startId.find('_') + 1));
            int endIdx = stoi(endId.substr(endId.find('_') + 1));

            for (int i = startIdx; i <= endIdx; ++i) {
                idsToClear.push_back(floor + "_" + to_string(i));
            }
        }
        else {
            cout << "Invalid choice\n";
            return;
        }

        // Clear the specified spots and corresponding customer information
        for (const string& id : idsToClear) {
            auto it = find_if(spots.begin(), spots.end(), [&id](const ParkingSpot& spot) {
                return spot.id == id;
                });

            if (it != spots.end()) {
                string plateNumber = it->plateNumber;

                it->isOccupied = false;
                it->vehicleType = "";
                it->plateNumber = "";
                it->startTime = 0;
                it->entrance = 0;
                cout << "Occupation for spot " << id << " cleared successfully\n";

                // Clear customer information
                if (!plateNumber.empty() && customers.find(plateNumber) != customers.end()) {
                    customers[plateNumber].startTime = 0;
                    customers[plateNumber].parkingType = "";
                    customers[plateNumber].vehicleType = "";
                    customers[plateNumber].entrance = 0;
                }
            }
            else {
                cout << "Invalid spot ID: " << id << "\n";
            }
        }
    }
    else {
        cout << "Invalid floor\n";
    }

    saveData();
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}


void viewCustomerInformation() {
    clearScreen();
    cout << "Customer Information:\n";
    time_t currentTime = time(nullptr); // 获取当前时间

    for (const auto& customer : customers) {
        cout << "Plate Number: " << customer.first << "\n";
        cout << "Vehicle Type: " << customer.second.vehicleType << "\n";

        if (customer.second.endTime == 0) {
            double totalHours = ceil(difftime(currentTime, customer.second.startTime) / 3600.0); // 计算停车总时长（向上取整到小时）
            double rate = hourlyRates[customer.second.parkingType]["Default"];
            double initialPayment = totalHours * rate;

            // 添加每过6小时20%的附加费
            int sixHourIntervals = static_cast<int>(totalHours) / 6;
            double surcharge = 0.0;
            if (sixHourIntervals > 0) {
                for (int i = 1; i <= sixHourIntervals; ++i) {
                    surcharge += 6 * rate * 0.2 * i;
                }
                double remainingHours = totalHours - (sixHourIntervals * 6);
                surcharge += remainingHours * rate * 0.2 * sixHourIntervals;
            }

            double payment = initialPayment + surcharge;
            if (payment > dailyMaxRate) {
                payment = dailyMaxRate;
            }

            cout << "Current parking duration: " << totalHours << " hours\n";
            cout << "Current estimated payment due: $" << fixed << setprecision(2) << payment << "\n";
            cout << "End Time: Not yet departed\n"; // 显示未离开
        }
        else {
            char buffer[26];
            ctime_s(buffer, sizeof(buffer), &customer.second.startTime);
            cout << "Start Time: " << buffer;
            ctime_s(buffer, sizeof(buffer), &customer.second.endTime);
            cout << "End Time: " << buffer;
            cout << "Payment: $" << fixed << setprecision(2) << customer.second.payment << "\n";
        }

        cout << "--------------------------\n";
    }
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void addCustomerInformation() {
    clearScreen();
    Customer newCustomer;
    cout << "Enter plate number: ";
    cin >> newCustomer.plateNumber;

    // Check for duplicate plate number
    if (customers.find(newCustomer.plateNumber) != customers.end()) {
        cout << "Customer with this plate number already exists.\n";
        cout << "Press any key to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    cout << "Enter vehicle type: ";
    cin >> newCustomer.vehicleType;

    // Set default values for other fields
    newCustomer.startTime = 0;
    newCustomer.endTime = 0;
    newCustomer.parkingType = "";
    newCustomer.entrance = 0;
    newCustomer.exit = 0;
    newCustomer.payment = 0.0;

    customers[newCustomer.plateNumber] = newCustomer;
    saveData();
    cout << "Customer information added successfully\n";
    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void deleteCustomerInformation() {
    clearScreen();
    cout << "Customer Information:\n";

    // Display all customer information
    for (const auto& customer : customers) {
        cout << "Plate Number: " << customer.first << "\n";
        cout << "Vehicle Type: " << customer.second.vehicleType << "\n";
        cout << "--------------------------\n";
    }

    string plateNumber;
    cout << "Enter plate number to delete: ";
    cin >> plateNumber;

    auto it = customers.find(plateNumber);
    if (it != customers.end()) {
        // Display customer information before deletion
        cout << "Customer Information:\n";
        cout << "Plate Number: " << it->second.plateNumber << "\n";
        cout << "Vehicle Type: " << it->second.vehicleType << "\n";
        cout << "--------------------------\n";

        char confirm;
        cout << "Are you sure you want to delete this customer? (y/n): ";
        cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            // Clear the parking spot occupation
            for (auto& floor : parkingLots) {
                for (auto& spot : floor.second) {
                    if (spot.plateNumber == plateNumber) {
                        spot.isOccupied = false;
                        spot.vehicleType = "";
                        spot.plateNumber = "";
                        spot.startTime = 0;
                        spot.entrance = 0;
                        break;
                    }
                }
            }

            customers.erase(it);
            saveData();
            cout << "Customer information deleted successfully\n";
        }
        else {
            cout << "Deletion cancelled\n";
        }
    }
    else {
        cout << "Customer not found\n";
    }

    cout << "Press any key to continue...";
    cin.ignore();
    cin.get();
}

void manageCustomerInformation() {
    int choice;
    do {
        clearScreen();
        cout << "Customer Management System\n";
        cout << "1. View Customer Information\n";
        cout << "2. Add Customer Information\n";
        cout << "3. Delete Customer Information\n";
        cout << "0. Exit\n";
        cout << "Please choose: ";
        cin >> choice;

        switch (choice) {
        case 1:
            viewCustomerInformation();
            break;
        case 2:
            addCustomerInformation();
            break;
        case 3:
            deleteCustomerInformation();
            break;
        case 0:
            break;
        default:
            cout << "Invalid choice\n";
        }
    } while (choice != 0);
}



void searchAvailableSpots() {
    clearScreen();

    // Display available vehicle types from file
    set<string> availableVehicleTypes;
    for (const auto& type : parkingTypeToVehicleTypes) {
        for (const auto& vehicle : type.second) {
            availableVehicleTypes.insert(vehicle);
        }
    }

    cout << "Available vehicle types: ";
    for (const auto& vehicle : availableVehicleTypes) {
        cout << vehicle << " ";
    }
    cout << "\nEnter vehicle type: ";
    string vehicleType;
    cin >> vehicleType;

    if (availableVehicleTypes.find(vehicleType) == availableVehicleTypes.end()) {
        cout << "Invalid vehicle type\n";
        cout << "Press any key to continue...";
        cin.ignore();
        cin.get();
        return;
    }

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
    while (true) {
        clearScreen();

        // Display available floors and their available spots
        cout << "Available floors and spots:\n";
        for (const auto& floor : parkingLots) {
            cout << "Floor: " << floor.first << "\n";
            int count = 0;
            for (const auto& spot : floor.second) {
                if (!spot.isOccupied) {
                    cout << "  ID: " << spot.id << ", Type: " << spot.type << "  ";
                    if (++count % 3 == 0) {
                        cout << "\n";
                    }
                }
            }
            if (count % 3 != 0) {
                cout << "\n"; // Ensure a new line if the last line isn't complete
            }
        }

        // Display available vehicle types from file, grouped by parking type
        cout << "Available vehicle types by parking type:\n";
        for (const auto& type : parkingTypeToVehicleTypes) {
            cout << type.first << ": ";
            for (const auto& vehicle : type.second) {
                cout << vehicle << " ";
            }
            cout << "\n";
        }

        // Get user input
        string floor, spotId, vehicleType;
        int entrance;
        cout << "Enter floor you want (e.g., B1, B2): ";
        cin >> floor;
        cout << "Enter spot ID you want: ";
        cin >> spotId;
        cout << "Enter entrance you enter in (1 or 2): ";
        cin >> entrance;
        cout << "Enter your vehicle type: ";
        cin >> vehicleType;

        // Validate parking type and vehicle type
        if (parkingLots.find(floor) == parkingLots.end()) {
            cout << "Invalid floor. Please try again.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));  // Pause for 2 seconds
            continue;
        }

        auto& spots = parkingLots[floor];
        auto it = find_if(spots.begin(), spots.end(), [&spotId](const ParkingSpot& spot) {
            return spot.id == spotId;
            });

        if (it == spots.end() || it->isOccupied) {
            cout << "Invalid spot ID or the spot is already occupied. Please try again.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));  // Pause for 2 seconds
            continue;
        }

        string type = it->type;
        if (parkingTypeToVehicleTypes.find(type) == parkingTypeToVehicleTypes.end() ||
            parkingTypeToVehicleTypes[type].find(vehicleType) == parkingTypeToVehicleTypes[type].end()) {
            cout << "Invalid parking type or vehicle type. Please try again.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));  // Pause for 2 seconds
            continue;
        }

        // Rent the parking spot
        it->isOccupied = true;
        it->vehicleType = vehicleType;
        it->plateNumber = currentPlateNumber;
        it->startTime = time(nullptr);
        it->entrance = entrance;
        customers[currentPlateNumber].startTime = it->startTime;
        customers[currentPlateNumber].entrance = entrance;
        customers[currentPlateNumber].parkingType = type;  // Ensure parking type is recorded correctly
        customers[currentPlateNumber].vehicleType = vehicleType;
        saveData();
        cout << "Parking spot rented successfully\n";

        cout << "Press any key to return to the customer menu...";
        cin.ignore();
        cin.get();
        break;
    }
}

void settleParkingFee() {
    clearScreen();

    // Ensure data is up-to-date by loading from file
    loadData();

    if (customers.find(currentPlateNumber) == customers.end()) {
        cout << "No such customer. Press any key to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    Customer& customer = customers[currentPlateNumber];
    customer.endTime = time(nullptr);
    double totalHours = ceil(difftime(customer.endTime, customer.startTime) / 3600.0); // Round up to nearest hour
    double rate = hourlyRates[customer.parkingType]["Default"];
    double initialPayment = totalHours * rate;

    // Add surcharge for each 6-hour interval
    int sixHourIntervals = static_cast<int>(totalHours) / 6;
    double surcharge = 0.0;
    if (sixHourIntervals > 0) {
        for (int i = 1; i <= sixHourIntervals; ++i) {
            surcharge += 6 * rate * 0.2 * i;
        }
        // Add surcharge for the remaining hours after the last 6-hour interval
        double remainingHours = totalHours - (sixHourIntervals * 6);
        surcharge += remainingHours * rate * 0.2 * sixHourIntervals;
    }

    customer.payment = initialPayment + surcharge;

    // Apply daily max rate
    if (customer.payment > dailyMaxRate) {
        customer.payment = dailyMaxRate;
    }

    cout << "Total hours parked: " << totalHours << "\n";
    cout << "Total payment due: $" << customer.payment << "\n";

    char choice;
    cout << "Do you want to proceed with the payment? (y/n): ";
    cin >> choice;
    if (choice != 'y' && choice != 'Y') {
        cout << "Payment cancelled. Press any key to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    string cardNumber, password;
    while (true) {
        cout << "Enter your 10-digit card number: ";
        cin >> cardNumber;
        cout << "Enter your 6-digit password: ";
        cin >> password;

        if (cardNumber.length() == 10 && all_of(cardNumber.begin(), cardNumber.end(), ::isdigit) &&
            password.length() == 6 && all_of(password.begin(), password.end(), ::isdigit)) {
            cout << "Payment processed successfully. Receipt is being printed...\n";
            break;
        }
        else {
            cout << "Invalid card number or password. Please try again.\n";
        }
    }

    int exit;
    while (true) {
        cout << "Enter exit number you will leave from (1 or 2): ";
        cin >> exit;

        if (exit == 1 || exit == 2) {
            customer.exit = exit;
            break;
        }
        else {
            cout << "Invalid exit number. Please enter 1 or 2.\n";
        }
    }

    for (auto& floor : parkingLots) {
        for (auto& spot : floor.second) {
            if (spot.plateNumber == currentPlateNumber) {
                spot.isOccupied = false;
                spot.vehicleType = "";
                spot.plateNumber = "";
                spot.startTime = 0;
                break;
            }
        }
    }

    customers.erase(currentPlateNumber);
    saveData();
    cout << "Payment settled and receipt printed\n";
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
