#include <iostream>

#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/time.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/visualization/cloud_viewer.h>

#include <opencv2/opencv.hpp>
#include "Classifier.h"
#include "Edges.h"
#include "GraphSegmentation.h"
#include "Microsoft_grabber2.h"

#include <Shlwapi.h>
#include <string.h>

#define NUM_LABELS 894

void BuildRFClassifier(std::string direc);
void BuildDataset(std::string direc);
//void TestRFClassifier(std::string direc);