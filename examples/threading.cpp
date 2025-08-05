#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <functional>
#include <memory>

#include <robomaster/can_streambuf.hpp>
#include <robomaster/chassis.hpp>
#include <robomaster/dds.hpp>

using robomaster::metadata;
using robomaster::wheel_encoders;
using std::placeholders::_1;
using std::placeholders::_2;

class MinimalRobotController
{
private:
    // Robot hardware interfaces
    std::unique_ptr<robomaster::chassis> chassis;
    std::unique_ptr<robomaster::dds> dds;
    std::unique_ptr<std::iostream> can_in, can_out;
    std::unique_ptr<robomaster::can_streambuf> can_in_buf, can_out_buf;

    // Shared data with thread synchronization
    std::mutex data_mutex;
    std::mutex can_mutex;
    std::vector<int16_t> current_motor_rpms;

    // Threading
    std::atomic<bool> running;
    std::thread control_thread;

    // Constants
    static constexpr int MAIN_LOOP_MS = 10;
    static constexpr int CONTROL_LOOP_MS = 100;

public:
    MinimalRobotController() :
        current_motor_rpms(4, 0),
        running(false)
    {
    }

    ~MinimalRobotController()
    {
        shutdown();
    }

    bool initialize()
    {
        try
        {
            std::cout << "Initializing CAN communication..." << std::endl;

            // Initialize CAN communication
            can_in_buf = std::make_unique<robomaster::can_streambuf>("can0", 0x202);
            can_out_buf = std::make_unique<robomaster::can_streambuf>("can0", 0x201);
            can_in = std::make_unique<std::iostream>(can_in_buf.get());
            can_out = std::make_unique<std::iostream>(can_out_buf.get());

            std::cout << "Initializing chassis..." << std::endl;

            // Initialize chassis
            chassis = std::make_unique<robomaster::chassis>(* can_out);
            {
                std::lock_guard<std::mutex> lock(can_mutex);
                chassis->send_workmode(1);
            }

            std::cout << "Initializing DDS..." << std::endl;

            // Initialize DDS with encoder callback
            dds = std::make_unique<robomaster::dds>(* can_in, * can_out);
            {
                std::lock_guard<std::mutex> lock(can_mutex);
                dds->subscribe(
                    std::function<void(const metadata&, const wheel_encoders&)>(
                    std::bind(&MinimalRobotController::cb_wheel_enc, this, _1, _2)
                    ),
                    20
                );
            }

            std::cout << "Robot controller initialized successfully" << std::endl;
            return true;

        }
        catch (const std::exception& e)
        {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            return false;
        }
    }

    void run()
    {
        if (!running.exchange(true))
        {
            // Start worker thread (same as your control_thread)
            control_thread = std::thread(&MinimalRobotController::controlLoop, this);

            std::cout << "Robot controller started" << std::endl;

            // Main control loop (same as your mainLoop)
            mainLoop();
        }
    }

    void shutdown()
    {
        if (running.exchange(false))
        {
            std::cout << "Shutting down..." << std::endl;

            // Stop all motion first
            if (chassis)
            {
                std::lock_guard<std::mutex> lock(can_mutex);
                chassis->send_wheel_speed(0, 0, 0, 0);
            }

            // Stop thread
            if (control_thread.joinable())
            {
                control_thread.join();
            }

            std::cout << "Robot controller shutdown complete" << std::endl;
        }
    }

private:
    void cb_wheel_enc(const metadata& /*meta*/, const wheel_encoders& wheel_encoders)
    {
        std::lock_guard<std::mutex> lock(data_mutex);

        current_motor_rpms[0] = wheel_encoders.rpm[0];
        current_motor_rpms[1] = -wheel_encoders.rpm[1];
        current_motor_rpms[2] = -wheel_encoders.rpm[2];
        current_motor_rpms[3] = wheel_encoders.rpm[3];

        // Simple logging
        std::cout << "Encoder callback: RPMs = ["
            << current_motor_rpms[0] << ", "
            << current_motor_rpms[1] << ", "
            << current_motor_rpms[2] << ", "
            << current_motor_rpms[3] << "]" << std::endl;
    }

    void controlLoop()
    {
        int counter = 0;

        while (running)
        {
            // Simple alternating wheel speeds for testing
            int16_t speed = (counter % 4 == 0) ? 50 : 0;

            std::cout << "Control loop: sending wheel speed " << speed << std::endl;

            // Send wheel speeds to chassis
            if (chassis)
            {
                std::lock_guard<std::mutex> lock(can_mutex);
                chassis->send_wheel_speed(speed, speed, speed, speed);
            }

            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(CONTROL_LOOP_MS));
        }
    }

    void mainLoop()
    {
        int heartbeat_counter = 0;

        while (running)
        {
            // Send heartbeat to chassis
            if (chassis)
            {
                if (heartbeat_counter % 100 == 0)    // Log every second
                {
                    std::cout << "Sending heartbeat..." << std::endl;
                }

                std::lock_guard<std::mutex> lock(can_mutex);
                chassis->send_heartbeat();
            }

            heartbeat_counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_MS));
        }
    }
};

int main(int, char**)
{
    try
    {
        MinimalRobotController controller;

        if (!controller.initialize())
        {
            std::cerr << "Failed to initialize robot controller" << std::endl;
            return 1;
        }

        std::cout << "Starting minimal robot controller test." << std::endl;
        std::cout << "Press Ctrl+C to stop." << std::endl;

        controller.run();

    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}