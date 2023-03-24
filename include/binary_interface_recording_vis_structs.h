/*
 * Copyright © 2021 Bosch Rexroth AG
*/

#ifndef BINARYINTERFACERECORDINGVISUALIZATIONSTRUCTS_H
#define BINARYINTERFACERECORDINGVISUALIZATIONSTRUCTS_H

#include <cstdint>
#include <vector>

struct __attribute__ ((packed)) Position2D
{
    float x{0.0f};
    float y{0.0f};
};
struct __attribute__ ((packed)) Pose2D
{
    float x{0.0f};
    float y{0.0f};
    float yaw{0.0f};
};
struct __attribute__ ((packed)) PointCluster
{
    double x{0.0f};
    double y{0.0f};
    double yaw{0.0f};
    uint32_t from{0};
    uint32_t to{0};
};

/*
 * structure containing the exact message from the ClientRecordingVisualization Interface
 */
struct RecordingVisualizationMessage
{
    double timestamp{0.};
    uint64_t visualizationId{0};
    int32_t status{0};
    double poseX{0.};
    double poseY{0.};
    double poseYaw{0.};
    double distanceToLastLC{0.};
    double delay{0.};
    double progress{0.};
    std::vector<Position2D> scan{};
    std::vector<Pose2D> pathPoses{};
    std::vector<int32_t> pathTypes{};
    std::vector<uint64_t> sensorOffsets{};
    bool hasIntensities{false};
    float minIntensity{0.f};
    float maxIntensity{0.f};
    std::vector<float> intensities{};
    std::vector<PointCluster> reflectors{};
};

#endif /* BINARYINTERFACERECORDINGVISUALIZATIONSTRUCTS_H */
