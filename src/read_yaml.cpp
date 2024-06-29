#include <iostream>
#include <fstream>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <ros/ros.h>
#include <ros/package.h>

#include <visualization_msgs/Marker.h>

int main(int argc, char **argv) {
    // Initialize the ROS node
    ros::init(argc, argv, "read_yaml_node");
    ros::NodeHandle nh;
    ROS_INFO("ROS IS WORKING"); 

    // Get the path to the package
    std::string package_path = ros::package::getPath("read_yaml");

    // Construct the full path to the YAML file
    std::string yaml_file = package_path + "/src/areas.yaml";

    // Load the YAML file
    YAML::Node config = YAML::LoadFile(yaml_file);

    ROS_INFO("File is reading");

    // Declare list for storing x, y
    std::vector<int> x;
    std::vector<int> y;

    // Extract item from key
    for (const auto& item: config["bedroom"]){
        ROS_INFO("Extracting item!");
        x.push_back(item[0].as<int>());
        y.push_back(item[1].as<int>());
    }

    // Check if the lists are not empty
    if (x.empty() || y.empty()) {
        ROS_ERROR("Empty list detected. Exiting.");
        return -1;
    }

    if (false) {
        // Output x list
        std::cout << "LIST OF X: ";
        for(const auto& val: x){
            std::cout << val << " ";
        } std::cout << "\n";

        // Output y list
        std::cout << "LIST OF Y: ";
        for(const auto& val: y){
            std::cout << val << " ";
        } std::cout << "\n";
    }

    // Initialize min max values
    int min_x = x[0];
    int max_x = x[0];
    int min_y = y[0];
    int max_y = y[0];

    // Find min & max x
    for(const auto& val: x) {
        if (val < min_x) min_x = val;
        if (val > max_x) max_x = val;
    }
    // Find min & max y
    for(const auto& val: y) {
        if (val < min_y) min_y = val;
        if (val > max_y) max_y = val;
    }
    ROS_INFO("min_x: %d, max_x: %d, min_y: %d, max_y: %d", min_x, max_x, min_y, max_y);

    // Initialize accumulation
    float acc_x = 0;
    float acc_y = 0;

    // Calculate the center point
    for (const auto& val: x) {
        acc_x += val;
    }
    for (const auto& val: y) {
        acc_y += val;
    }
    float cen_x = acc_x / x.size();
    float cen_y = acc_y / y.size();
    ROS_INFO("center point x: %f, y: %f", cen_x, cen_y);

    // Draw obstacle
    ros::Publisher marker_pub = nh.advertise<visualization_msgs::Marker>("obstacle_marker", 1);
    // std::vector<visualization_msgs::Marker> obstacle_makers;
    
    visualization_msgs::Marker marker;
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();
    marker.ns = "obstacle";
    marker.id = 0;
    marker.type = visualization_msgs::Marker::CUBE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = cen_x;
    marker.pose.position.y = cen_y;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = (max_x - min_x) / 2.0;
    marker.scale.y = (max_y - min_y) / 2.0;
    marker.scale.z = 1; // Obstacle height
    marker.color.a = 1.0; // Don't forget to set the alpha!
    marker.color.r = 1.0;
    marker.color.g = 0.0;
    marker.color.b = 0.0;
    marker.lifetime = ros::Duration(); // The marker will be persistent
    ROS_INFO("Prepare to pub!");
    ros::Rate rate(1); // 1 Hz
    while (ros::ok()) {
        marker.header.stamp = ros::Time::now();
        marker_pub.publish(marker);
        rate.sleep();
    }
    return 0;
}
