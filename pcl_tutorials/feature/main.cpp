#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <pcl/io/pcd_io.h>
#include "pcl/common/io.h"
#include "pcl/point_cloud.h"
#include "pcl/point_types.h"
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/time.h>
#include "boost/shared_ptr.hpp"

using namespace std;

#include "featureNormalEstimation.h"
#include "featureFPH.h"
#include "../feature/featureRoPS.h"
#include "featureTriangulation.h"	//gp3

#include "../keypoint/KeypointUniformSampling.h"
#include "featureSHOT.h"

//#define PFHSignature
//#define FPFHSignature
//#define SHOTSignature

int main()
{

	pcl::PointCloud<pcl::PointXYZ>::Ptr in_cloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr keypoint(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::Normal>::Ptr normal(new pcl::PointCloud<pcl::Normal>);
	std::vector <pcl::Vertices> triangles;
	
	pcl::console::TicToc time;time.tic();
	if (pcl::io::loadPCDFile("../data/milk.pcd", *in_cloud) == -1){
		std::cout << "load pcd file failed!\n";getchar();return -1;}
	std::cout << "loadPCDFile Function Time: " << time.toc() / 1000 << "s" << std::endl;

	getUniformSampling(in_cloud, keypoint, 0.01);
	getNormalEstimation(keypoint, normal);

	//getNormal(in_cloud, normal);
	//getNormalEstimation(in_cloud, normal);
	//computeROPS();
	//getGP3(in_cloud);
	getTriangulation(keypoint, normal, triangles);


	pcl::PointCloud<pcl::Histogram <135> >::Ptr histograms(new pcl::PointCloud <pcl::Histogram <135> >());
	histograms = getRoPS(triangles, keypoint, normal, keypoint);
	

#ifdef PFHSignature
	pcl::PointCloud<pcl::PFHSignature125>::Ptr pfhSignature125(new pcl::PointCloud<pcl::PFHSignature125>);
	pfhSignature125 = getPFH(in_cloud);
#endif 

#ifdef FPFHSignature
	pcl::PointCloud<pcl::FPFHSignature33>::Ptr fpfhSignature33(new pcl::PointCloud<pcl::FPFHSignature33>);
	fpfhSignature33 = getFPFHOMP(in_cloud,keypoint,normal);
#endif 

#ifdef SHOTSignature
	// 不是histogram，不可以可视化
	pcl::PointCloud<pcl::SHOT352>::Ptr shot352(new pcl::PointCloud<pcl::SHOT352>);
	shot352 = getSHOTOMP(keypoint, normal, keypoint, 0.05);
#endif

	// 可视化

	pcl::visualization::PCLPlotter plotter;
	plotter.addFeatureHistogram(*histograms, 300); //设置的很坐标长度，该值越大，则显示的越细致
	plotter.plot();
	

	


	getchar();
	return 0;
}
