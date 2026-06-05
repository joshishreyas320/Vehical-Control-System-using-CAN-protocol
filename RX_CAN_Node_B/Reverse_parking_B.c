#include <lpc21xx.h>
#include "header.h"

// Global variables
int flag = 0;
CAN2 data;
CAN2 sensor_left, sensor_center, sensor_right;

// Alert level enumeration
enum AlertLevel {
    ALERT_NONE = 0,
    ALERT_INFO = 1,
    ALERT_WARNING = 2,
    ALERT_CRITICAL = 3
};

// Parking data structure
typedef struct {
    u32 min_distance;
    u8 closest_sensor;
    enum AlertLevel alert_level;
    u32 timestamp;
} ParkingData;

// Function prototypes
void init_gpio(void);
void init_system(void);
void process_sensor_data(void);
u16 convert_can_data_to_distance(u32 byteA, u32 byteB);
enum AlertLevel calculate_alert_level(u16 distance);
void generate_alert(enum AlertLevel level, u16 distance);
void send_parking_data_can(ParkingData *parking_data);
void display_parking_status(ParkingData *parking_data);
void buzzer_on(void);
void buzzer_off(void);
void set_led(u8 color);

// LED pins: P0.0 (Green), P0.1 (Yellow), P0.2 (Red)
#define LED_GREEN   (1 << 0)
#define LED_YELLOW  (1 << 1)
#define LED_RED     (1 << 2)
#define LED_MASK    (LED_GREEN | LED_YELLOW | LED_RED)

// Buzzer pin: P0.3
#define BUZZER_PIN  (1 << 3)

// Sensor IDs from CAN message
#define SENSOR_LEFT_ID     0x100
#define SENSOR_CENTER_ID   0x101
#define SENSOR_RIGHT_ID    0x102
#define PARKING_DATA_ID    0x200

// Distance thresholds in cm
#define CRITICAL_DISTANCE  30
#define WARNING_DISTANCE   60
#define INFO_DISTANCE      150

int main(void)
{
    ParkingData parking_result;
    static u32 last_update_time = 0;
    static u32 loop_count = 0;
    
    // Initialize system
    init_system();
    
    // Display startup message
    uart0_tx_string("\r\n================================\r\n");
    uart0_tx_string("Reverse Parking Assistance System\r\n");
    uart0_tx_string("LPC2129 - Node B (Receiver)\r\n");
    uart0_tx_string("================================\r\n");
    
    lcd_init();
    lcd_cmd(0x01);  // Clear display
    lcd_string("Parking System");
    delay_ms(1000);
    
    // Main loop
    while(1)
    {
        loop_count++;
        
        // Check if CAN data received
        if(flag == 1)
        {
            flag = 0;  // Reset flag
            
            // Store sensor data based on CAN ID
            if(data.id == SENSOR_LEFT_ID)
            {
                sensor_left = data;
                uart0_tx_string("Left Sensor Data Received\r\n");
            }
            else if(data.id == SENSOR_CENTER_ID)
            {
                sensor_center = data;
                uart0_tx_string("Center Sensor Data Received\r\n");
            }
            else if(data.id == SENSOR_RIGHT_ID)
            {
                sensor_right = data;
                uart0_tx_string("Right Sensor Data Received\r\n");
            }
        }
        
        // Process parking data every 100ms (adjust as needed)
        if(loop_count >= 10)
        {
            process_sensor_data();
            
            // Find minimum distance from all sensors
            u16 dist_left = convert_can_data_to_distance(sensor_left.byteA, sensor_left.byteB);
            u16 dist_center = convert_can_data_to_distance(sensor_center.byteA, sensor_center.byteB);
            u16 dist_right = convert_can_data_to_distance(sensor_right.byteA, sensor_right.byteB);
            
            // Determine minimum distance and closest sensor
            parking_result.min_distance = dist_left;
            parking_result.closest_sensor = 0;  // Left
            
            if(dist_center < parking_result.min_distance)
            {
                parking_result.min_distance = dist_center;
                parking_result.closest_sensor = 1;  // Center
            }
            
            if(dist_right < parking_result.min_distance)
            {
                parking_result.min_distance = dist_right;
                parking_result.closest_sensor = 2;  // Right
            }
            
            // Calculate alert level
            parking_result.alert_level = calculate_alert_level(parking_result.min_distance);
            parking_result.timestamp = loop_count;
            
            // Generate alerts (visual and audio)
            generate_alert(parking_result.alert_level, parking_result.min_distance);
            
            // Send parking data via CAN (if needed)
            send_parking_data_can(&parking_result);
            
            // Display status
            display_parking_status(&parking_result);
            
            loop_count = 0;
        }
        
        delay_ms(10);
    }
    
    return 0;
}

/**
 * Initialize system (GPIO, UART, CAN, Interrupts)
 */
void init_system(void)
{
    // Initialize GPIO
    init_gpio();
    
    // Initialize UART0 for debugging (115200 baud)
    uart0_init(115200);
    
    // Initialize CAN2 interface
    can2_init();
    
    // Configure VIC for CAN2 interrupts
    config_vic_Can2();
    
    // Enable CAN2 interrupts
    en_can2_interr();
}

/**
 * Initialize GPIO pins for LEDs and Buzzer
 */
void init_gpio(void)
{
    // Configure P0.0-P0.3 as outputs (GPIO)
    IODIR0 |= LED_GREEN | LED_YELLOW | LED_RED | BUZZER_PIN;
    
    // Clear all outputs initially
    IOCLR0 = LED_GREEN | LED_YELLOW | LED_RED | BUZZER_PIN;
}

/**
 * Process sensor data (placeholder for future enhancements)
 */
void process_sensor_data(void)
{
    // Add filtering, averaging, or validation logic here if needed
}

/**
 * Convert CAN message data to distance in cm
 * Assumes 2 bytes for distance value
 */
u16 convert_can_data_to_distance(u32 byteA, u32 byteB)
{
    u16 distance;
    
    // Extract distance from CAN data bytes
    // Assuming distance is in first 2 bytes of byteA
    distance = (u16)((byteA >> 8) & 0xFF);
    distance |= (u16)((byteA & 0xFF) << 8);
    
    // Convert to cm (adjust conversion factor based on sensor calibration)
    // Example: if raw value represents distance in units of 0.5cm
    distance = (distance * 50) / 100;  // Convert to cm
    
    return distance;
}

/**
 * Calculate alert level based on distance
 */
enum AlertLevel calculate_alert_level(u16 distance)
{
    if(distance < CRITICAL_DISTANCE)
    {
        return ALERT_CRITICAL;
    }
    else if(distance < WARNING_DISTANCE)
    {
        return ALERT_WARNING;
    }
    else if(distance < INFO_DISTANCE)
    {
        return ALERT_INFO;
    }
    else
    {
        return ALERT_NONE;
    }
}

/**
 * Generate visual and audio alerts
 */
void generate_alert(enum AlertLevel level, u16 distance)
{
    static u32 last_beep_time = 0;
    static u8 beep_active = 0;
    u32 current_time = 0;  // Use system timer if available
    u32 beep_interval = 500;
    
    // Clear all LEDs
    IOCLR0 = LED_MASK;
    
    switch(level)
    {
        case ALERT_NONE:
            buzzer_off();
            break;
            
        case ALERT_INFO:
            // Green LED + slow beep (1 beep per second)
            set_led(LED_GREEN);
            beep_interval = 1000;
            buzzer_on();
            delay_ms(100);
            buzzer_off();
            break;
            
        case ALERT_WARNING:
            // Yellow LED + medium beep (400ms interval)
            set_led(LED_YELLOW);
            beep_interval = 400;
            buzzer_on();
            delay_ms(150);
            buzzer_off();
            break;
            
        case ALERT_CRITICAL:
            // Red LED + fast beep (200ms interval)
            set_led(LED_RED);
            beep_interval = 200;
            buzzer_on();
            delay_ms(150);
            buzzer_off();
            break;
    }
}

/**
 * Send parking data via CAN bus
 */
void send_parking_data_can(ParkingData *parking_data)
{
    CAN2 tx_msg;
    
    // Pack parking data into CAN message
    tx_msg.id = PARKING_DATA_ID;
    tx_msg.dlc = 8;
    tx_msg.rtr = 0;
    
    // Pack data: distance (2 bytes) + sensor ID (1 byte) + alert level (1 byte) + timestamp (4 bytes)
    tx_msg.byteA = ((parking_data->min_distance & 0xFFFF) << 0) |
                   ((parking_data->closest_sensor & 0xFF) << 16) |
                   ((parking_data->alert_level & 0xFF) << 24);
    
    tx_msg.byteB = (parking_data->timestamp & 0xFFFFFFFF);
    
    // Note: Implement CAN transmission function based on your Node A setup
    // can2_tx(&tx_msg);
}

/**
 * Display parking status on UART and LCD
 */
void display_parking_status(ParkingData *parking_data)
{
    char sensor_name[10];
    
    // Determine sensor name
    switch(parking_data->closest_sensor)
    {
        case 0:
            uart0_tx_string("Closest: LEFT");
            break;
        case 1:
            uart0_tx_string("Closest: CENTER");
            break;
        case 2:
            uart0_tx_string("Closest: RIGHT");
            break;
        default:
            uart0_tx_string("Closest: UNKNOWN");
    }
    
    // Send distance
    uart0_tx_string(" | Distance: ");
    uart0_int(parking_data->min_distance);
    uart0_tx_string(" cm | Alert: ");
    
    // Send alert level
    switch(parking_data->alert_level)
    {
        case ALERT_NONE:
            uart0_tx_string("NONE");
            break;
        case ALERT_INFO:
            uart0_tx_string("INFO");
            break;
        case ALERT_WARNING:
            uart0_tx_string("WARNING");
            break;
        case ALERT_CRITICAL:
            uart0_tx_string("CRITICAL");
            break;
    }
    
    uart0_tx_string("\r\n");
    
    // Update LCD display
    lcd_cmd(0x01);  // Clear display
    lcd_string("Distance: ");
    lcd_integer(parking_data->min_distance);
    lcd_cmd(0xC0);  // Move to second line
    
    switch(parking_data->alert_level)
    {
        case ALERT_NONE:
            lcd_string("Safe");
            break;
        case ALERT_INFO:
            lcd_string("Info");
            break;
        case ALERT_WARNING:
            lcd_string("Warning!");
            break;
        case ALERT_CRITICAL:
            lcd_string("CRITICAL!");
            break;
    }
}

/**
 * Turn on buzzer
 */
void buzzer_on(void)
{
    IOSET0 = BUZZER_PIN;
}

/**
 * Turn off buzzer
 */
void buzzer_off(void)
{
    IOCLR0 = BUZZER_PIN;
}

/**
 * Set LED color
 */
void set_led(u8 color)
{
    IOCLR0 = LED_MASK;      // Clear all LEDs
    IOSET0 = color;          // Set selected LED
}