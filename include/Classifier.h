/*
Copyright (C) 2014 Steven Hickson

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/time.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/visualization/cloud_viewer.h>
#include <boost/filesystem.hpp>

#include "GraphSegmentation.h"

#include <Shlwapi.h>
#include <string.h>

#define NUM_CLUSTERS 20

const float parameters[] = { 0.5f,800.0f,400,0.8f,800.0f,400,300,0.3f };
const float normalParameters[] = { 0.0f,0.8f,25,100,100,0.1f };

cv::Mat imread_depth(const char* fname, bool binary=true);
cv::Mat imread_float(const char* fname, bool binary=true);
void imwrite_float(const char* fname, cv::Mat &img, bool binary=true);
void imwrite_depth(const char* fname, cv::Mat &img, bool binary=true);
void ConvertCloudtoGrayMat(const PointCloudBgr &in, cv::Mat &out);
void GetMatFromCloud(const PointCloudBgr &cloud, cv::Mat &img);
void GetMatFromCloud(const PointCloudInt &cloud, cv::Mat &img);
void EstimateNormals(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloud, pcl::PointCloud<pcl::PointNormal>::Ptr &normals, bool fill=false);

#define BLACK cv::Vec3b(0,0,0)
#define BLUE cv::Vec3b(255,0,0)
#define GREEN cv::Vec3b(0,255,0)
#define RED cv::Vec3b(0,0,255)
#define ALPHA 0.3f

class AnnotationData {
public:
	PointCloudInt labelCloud;
	PointCloudBgr segmentCloud, cloud;
	boost::shared_ptr<PointCloudNormal> normals;
	cv::Mat orig, img, label;
	int currLabel;
	boost::mutex mutex;

	static const int NUM_LABELS = 3;
	cv::Vec3b colors[NUM_LABELS];

	AnnotationData() : normals(new PointCloudNormal) {
		currLabel = 0;
		colors[0] = BLACK;
		colors[1] = RED;
		colors[2] = GREEN;
		colors[3] = BLUE;
	}

	~AnnotationData() {
	}

	void Lock() {
		mutex.lock();
	}

	void Unlock() {
		mutex.unlock();
	}
};


class Classifier {
public:
	int categories; //number of categories
	int clusters; //number of clusters for SURF features to build vocabulary
	std::string direc; //directory of data
	cv::Mat vocab; //vocabulary
	cv::Ptr<cv::FeatureDetector> featureDetector;
	cv::Ptr<cv::DescriptorExtractor> descriptorExtractor;
	cv::Ptr<cv::BOWKMeansTrainer> bowtrainer;
	cv::Ptr<cv::BOWImgDescriptorExtractor> bowDescriptorExtractor;
	cv::Ptr<cv::DescriptorMatcher> descriptorMatcher;
	CvRTrees* rtree;
	AnnotationData data;

	Classifier(std::string direc_) {
		direc = direc_;
		clusters = NUM_CLUSTERS;
		categories = 3;
		featureDetector = (new cv::SurfFeatureDetector());
		descriptorExtractor = (new cv::SurfDescriptorExtractor());
		bowtrainer = (new cv::BOWKMeansTrainer(clusters));
		descriptorMatcher = (new cv::FlannBasedMatcher());
		//descriptorMatcher = (new BFMatcher());
		bowDescriptorExtractor = (new cv::BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher));
	};

	void build_vocab(); //function to build the BOW vocabulary
	void load_vocab(); //function to load the BOW vocabulary and classifiers
	void load_classifier();
	void CalculateSIFTFeatures(const PointCloudBgr &cloud, cv::Mat &descriptors);
	void CalculateBOWFeatures(const PointCloudBgr &cloud, cv::Mat &descriptors);
	void Annotate();

	void InitializeTesting();
	void TestCloud(const PointCloudBgr &cloud);
	void CreateAugmentedCloud(PointCloudBgr::Ptr &out);
};

#endif