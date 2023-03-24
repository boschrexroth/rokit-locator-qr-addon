/*
 * File: BinaryInterfacePoseStruct.h
 * Created On: 2019-03-13
 * Copyright © 2019 Bosch Rexroth AG
*/

#ifndef BINARYINTERFACESTRUCTS_H
#define BINARYINTERFACESTRUCTS_H 

/*
 * structure containing the exact message from the ClientLocalizationPose Interface
 */
struct __attribute__ ((packed)) PoseMessage
{
    double   age                {0.};
    double   timestamp          {0.};
    uint64_t uniqueId           {0};
    int32_t  state              {0};
    double   poseX              {0.};
    double   poseY              {0.};
    double   poseYaw            {0.};
    double   covariance_1_1     {0.};
    double   covariance_1_2     {0.};
    double   covariance_1_3     {0.};
    double   covariance_2_2     {0.};
    double   covariance_2_3     {0.};
    double   covariance_3_3     {0.};
    double   poseZ              {0.};
    double   quaternion_w       {0.};
    double   quaternion_x       {0.};
    double   quaternion_y       {0.};
    double   quaternion_z       {0.};
    uint64_t epoch              {0};
    double   lidarOdoPoseX      {0.};
    double   lidarOdoPoseY      {0.};
    double   lidarOdoPoseYaw    {0.};
};

#endif /* BINARYINTERFACESTRUCTS_H */
