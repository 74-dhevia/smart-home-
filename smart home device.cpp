#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>
using namespace std;

// ----------- Base Class -----------
class SmartDevice {
protected:
    string deviceID, deviceName, status, location;

public:
    SmartDevice(string id, string name, string loc)
        : deviceID(id), deviceName(name), location(loc), status("OFF") {}

    virtual ~SmartDevice() {
        cout << "Destroying device: " << deviceID << endl;
    }

    virtual void performAction() = 0; // Polymorphism
    virtual void saveToFile(ofstream& out) const = 0;
    virtual void displayStatus() const {
        cout << "ID: " << deviceID << ", Name: " << deviceName
             << ", Status: " << status << ", Location: " << location << endl;
    }

    void toggleStatus() {
        status = (status == "ON") ? "OFF" : "ON";
        cout << deviceName << " is now " << status << endl;
    }
};

// ----------- Derived Class: SmartLight -----------
class SmartLight : public SmartDevice {
    int brightness;
    string color;

public:
    SmartLight(string id, string name, string loc, int bright, string col)
        : SmartDevice(id, name, loc), brightness(bright), color(col) {
        if (bright < 0 || bright > 100)
            throw out_of_range("Brightness must be between 0 and 100.");
    }

    void performAction() override {
        cout << "[" << deviceName << "] Enter brightness (0–100): ";
        cin >> brightness;
        if (brightness < 0 || brightness > 100)
            throw out_of_range("Brightness out of range!");

        cout << "Enter color: ";
        cin >> color;
        cout << "Light updated to brightness " << brightness << " and color " << color << endl;
    }

    void displayStatus() const override {
        SmartDevice::displayStatus();
        cout << "Brightness: " << brightness << ", Color: " << color << endl;
    }

    void saveToFile(ofstream& out) const override {
        cout << "SmartLight " << deviceID << " " << deviceName << " " << status
            << " " << location << " " << brightness << " " << color << endl;
    }
};

// ----------- Derived Class: SmartThermostat -----------
class SmartThermostat : public SmartDevice {
    float temperature;
    string mode;

public:
    SmartThermostat(string id, string name, string loc, float temp, string m)
        : SmartDevice(id, name, loc), temperature(temp), mode(m) {
        if (temp < 16 || temp > 30)
            throw out_of_range("Temperature must be between 16°C and 30°C.");
        if (mode != "Cool" && mode != "Heat")
            throw invalid_argument("Mode must be Cool or Heat.");
    }

    void performAction() override {
        cout << "[" << deviceName << "] Enter temperature (16–30 °C): ";
        cin >> temperature;
        if (temperature < 16 || temperature > 30)
            throw out_of_range("Temperature out of range!");

        cout << "Enter mode (Cool/Heat): ";
        cin >> mode;
        if (mode != "Cool" && mode != "Heat")
            throw invalid_argument("Invalid mode!");

        cout << "Thermostat set to " << temperature << "°C and mode " << mode << endl;
    }

    void displayStatus() const override {
        SmartDevice::displayStatus();
        cout << "Temperature: " << temperature << "°C, Mode: " << mode << endl;
    }

    void saveToFile(ofstream& out) const override {
        out << "SmartThermostat " << deviceID << " " << deviceName << " " << status
            << " " << location << " " << temperature << " " << mode << endl;
    }
};

// ----------- Template Class: DeviceManager -----------
template <typename T>
class DeviceManager {
    vector<shared_ptr<T>> devices;

public:
    void addDevice(shared_ptr<T> dev) {
        devices.push_back(dev);
    }

    void displayAll() const {
        cout << "\n--- Device List ---\n";
        for (const auto& dev : devices)
            dev->displayStatus();
    }

    void toggleAll() {
        for (auto& dev : devices)
            dev->toggleStatus();
    }

    void performActions() {
        for (auto& dev : devices) {
            try {
                dev->performAction();
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
        }
    }

    void saveAllToFile(const string& filename) const {
        ofstream out(filename);
        if (!out) {
            cerr << "Could not open file for writing.\n";
            return;
        }
        for (const auto& dev : devices)
            dev->saveToFile(out);
        cout << "Device data saved to " << filename << endl;
    }
};

// ----------- Main Function -----------
int main() {
    DeviceManager<SmartDevice> manager;

    try {
        auto light = make_shared<SmartLight>("L1", "LivingRoomLight", "Living Room", 75, "White");
        auto thermo = make_shared<SmartThermostat>("T1", "BedroomThermo", "Bedroom", 22.5, "Cool");

        manager.addDevice(light);
        manager.addDevice(thermo);

        manager.displayAll();
        manager.toggleAll();
        manager.performActions();
        manager.saveAllToFile("devices.txt");
    } catch (const exception& e) {
        cerr << "Setup Error: " << e.what() << endl;
    }

    return 0;
}
