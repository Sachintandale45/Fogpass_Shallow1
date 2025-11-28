#include <dbus/dbus.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <signal.h>

// D-Bus service details to listen to
#define DBUS_SERVICE_NAME "com.fogpass.power"
#define DBUS_OBJECT_PATH "/com/fogpass/power/battery"
#define DBUS_INTERFACE "com.fogpass.power.battery"
#define DBUS_SIGNAL_NAME "BatteryStatusChanged"

class GNSSReader {
private:
    DBusConnection* conn;
    bool running;
    
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
        
        std::cout << "Connected to D-Bus system bus" << std::endl;
        return true;
    }
    
    bool add_match_rule() {
        DBusError err;
        dbus_error_init(&err);
        
        // Add match rule to listen for battery status signals
        std::string match_rule = "type='signal',"
                                 "interface='" + std::string(DBUS_INTERFACE) + "',"
                                 "member='" + std::string(DBUS_SIGNAL_NAME) + "'";
        
        dbus_bus_add_match(conn, match_rule.c_str(), &err);
        if (dbus_error_is_set(&err)) {
            std::cerr << "Failed to add match rule: " << err.message << std::endl;
            dbus_error_free(&err);
            return false;
        }
        
        std::cout << "Added D-Bus match rule for battery status signals" << std::endl;
        return true;
    }
    
    void process_battery_message(DBusMessage* msg) {
        DBusMessageIter args;
        int batteryLevel;
        dbus_bool_t charging;
        
        // Check if message is a signal from the battery service
        if (dbus_message_is_signal(msg, DBUS_INTERFACE, DBUS_SIGNAL_NAME)) {
            if (!dbus_message_iter_init(msg, &args)) {
                std::cerr << "Message has no arguments" << std::endl;
                return;
            }
            
            // Read battery level (int32)
            if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_INT32) {
                std::cerr << "First argument is not an int32" << std::endl;
                return;
            }
            dbus_message_iter_get_basic(&args, &batteryLevel);
            dbus_message_iter_next(&args);
            
            // Read charging status (boolean)
            if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_BOOLEAN) {
                std::cerr << "Second argument is not a boolean" << std::endl;
                return;
            }
            dbus_message_iter_get_basic(&args, &charging);
            
            // Process the battery status
            std::cout << "[GNSS Reader] Received Battery Status - Level: " << batteryLevel 
                      << "%, Charging: " << (charging ? "Yes" : "No") << std::endl;
            
            // Here you can add logic to use battery status for GNSS operations
            // For example, adjust power consumption based on battery level
            if (batteryLevel < 20 && !charging) {
                std::cout << "[GNSS Reader] Warning: Low battery! Consider power-saving mode." << std::endl;
            }
        }
    }
    
    void listen_for_messages() {
        DBusMessage* msg;
        
        while (running) {
            // Non-blocking read
            dbus_connection_read_write(conn, 0);
            
            msg = dbus_connection_pop_message(conn);
            if (msg != nullptr) {
                process_battery_message(msg);
                dbus_message_unref(msg);
            } else {
                // No message available, sleep briefly
                usleep(100000); // 100ms
            }
        }
    }
    
public:
    GNSSReader() : conn(nullptr), running(false) {}
    
    ~GNSSReader() {
        stop();
    }
    
    bool start() {
        if (!init_dbus()) {
            return false;
        }
        
        if (!add_match_rule()) {
            dbus_connection_unref(conn);
            conn = nullptr;
            return false;
        }
        
        running = true;
        std::cout << "GNSS Reader started, listening for battery status..." << std::endl;
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
        listen_for_messages();
    }
};

// Signal handler for graceful shutdown
GNSSReader* g_gnss_reader = nullptr;

void signal_handler(int sig) {
    std::cout << "\nReceived signal " << sig << ", shutting down..." << std::endl;
    if (g_gnss_reader) {
        g_gnss_reader->stop();
    }
    exit(0);
}

int main(int argc, char** argv) {
    std::cout << "FogPASS GNSS Reader starting..." << std::endl;
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    GNSSReader reader;
    g_gnss_reader = &reader;
    
    if (!reader.start()) {
        std::cerr << "Failed to start GNSS reader" << std::endl;
        return 1;
    }
    
    // Run the reader
    reader.run();
    
    return 0;
}

