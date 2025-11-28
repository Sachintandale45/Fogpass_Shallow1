#include <dbus/dbus.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <chrono>

// D-Bus service details
#define DBUS_SERVICE_NAME "com.fogpass.power"
#define DBUS_OBJECT_PATH "/com/fogpass/power/battery"
#define DBUS_INTERFACE "com.fogpass.power.battery"
#define DBUS_SIGNAL_NAME "BatteryStatusChanged"

class BatteryService {
private:
    DBusConnection* conn;
    bool running;
    int batteryLevel;
    bool isCharging;
    
    bool init_dbus() {
        DBusError err;
        dbus_error_init(&err);
        
        // Connect to system bus
        conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
        if (dbus_error_is_set(&err)) {
            std::cerr << "D-Bus connection error: " << err.message << std::endl;
            dbus_error_free(&err);
            return false;
        }
        
        // Request service name
        int ret = dbus_bus_request_name(conn, DBUS_SERVICE_NAME,
                                       DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
        if (dbus_error_is_set(&err)) {
            std::cerr << "D-Bus name request error: " << err.message << std::endl;
            dbus_error_free(&err);
            dbus_connection_unref(conn);
            return false;
        }
        
        if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
            std::cerr << "Failed to acquire service name" << std::endl;
            dbus_connection_unref(conn);
            return false;
        }
        
        std::cout << "Battery service registered on D-Bus: " << DBUS_SERVICE_NAME << std::endl;
        return true;
    }
    
    void send_battery_signal(int level, bool charging) {
        DBusMessage* msg;
        DBusMessageIter args;
        dbus_uint32_t serial = 0;
        
        // Create signal message
        msg = dbus_message_new_signal(DBUS_OBJECT_PATH,
                                      DBUS_INTERFACE,
                                      DBUS_SIGNAL_NAME);
        if (msg == nullptr) {
            std::cerr << "Failed to create D-Bus signal message" << std::endl;
            return;
        }
        
        // Append signal arguments: battery level (int) and charging status (bool)
        dbus_message_iter_init_append(msg, &args);
        
        if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &level)) {
            std::cerr << "Failed to append battery level" << std::endl;
            dbus_message_unref(msg);
            return;
        }
        
        dbus_bool_t charging_bool = charging ? TRUE : FALSE;
        if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &charging_bool)) {
            std::cerr << "Failed to append charging status" << std::endl;
            dbus_message_unref(msg);
            return;
        }
        
        // Send the signal
        if (!dbus_connection_send(conn, msg, &serial)) {
            std::cerr << "Failed to send D-Bus signal" << std::endl;
            dbus_message_unref(msg);
            return;
        }
        
        dbus_connection_flush(conn);
        dbus_message_unref(msg);
        
        std::cout << "Sent battery status: Level=" << level << "%, Charging=" 
                  << (charging ? "Yes" : "No") << std::endl;
    }
    
    void simulate_battery() {
        batteryLevel = 75;  // Start at 75%
        isCharging = false;
        bool increasing = false;
        
        while (running) {
            // Simulate battery behavior
            if (isCharging) {
                batteryLevel += 2;
                if (batteryLevel >= 100) {
                    batteryLevel = 100;
                    isCharging = false;  // Stop charging when full
                }
            } else {
                batteryLevel -= 1;
                if (batteryLevel <= 20) {
                    isCharging = true;  // Start charging when low
                }
            }
            
            // Ensure battery level stays within bounds
            if (batteryLevel < 0) batteryLevel = 0;
            if (batteryLevel > 100) batteryLevel = 100;
            
            // Send signal with current battery status
            send_battery_signal(batteryLevel, isCharging);
            
            // Wait 2 seconds before next update
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
public:
    BatteryService() : conn(nullptr), running(false), batteryLevel(50), isCharging(false) {}
    
    ~BatteryService() {
        stop();
    }
    
    bool start() {
        if (!init_dbus()) {
            return false;
        }
        
        running = true;
        std::thread battery_thread(&BatteryService::simulate_battery, this);
        battery_thread.detach();
        
        std::cout << "Battery service started" << std::endl;
        return true;
    }
    
    void stop() {
        running = false;
        if (conn) {
            dbus_connection_unref(conn);
            conn = nullptr;
        }
    }
    
    void run() {
        // Keep the main thread alive and process D-Bus messages
        while (running) {
            dbus_connection_read_write_dispatch(conn, 100);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

int main(int argc, char** argv) {
    std::cout << "FogPASS Battery Service starting..." << std::endl;
    
    BatteryService service;
    if (!service.start()) {
        std::cerr << "Failed to start battery service" << std::endl;
        return 1;
    }
    
    // Run the service
    service.run();
    
    return 0;
}

