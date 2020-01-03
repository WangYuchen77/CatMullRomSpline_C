#include <iostream>  
#include <vector>
#include <map>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
 
using namespace std;    
using namespace cv;   

double tj(double ti, Point2d Pi, Point2d Pj){
    double t = sqrt(sqrt(pow((Pj.x - Pi.x),2) + pow((Pj.y - Pi.y),2))) + ti;    
    return t;
}
vector<Point2d> catMullRomSpline(vector<Point2d> inputPoints){
    int numPoints = inputPoints.size();
    int numSpace = 100; // 生成点的个数

    if (numPoints > 4){
        // 每四个点为一组的组数
        int numSplines = numPoints - 3;
        
        std::vector<Point2d> result;
        std::vector<Point2d> spline;
        
        // 对组数循环
        for (int i = 0 ; i<numSplines ; i++){
            std::vector<Point2d>::const_iterator first = inputPoints.begin() + i;
            std::vector<Point2d>::const_iterator last  = inputPoints.begin() + i+4;
            std::vector<Point2d> inputPoints_part(first, last);

            // 迭代进入 else if (numPoints == 4) 分支，返回每四个点中，2、3点的插值点
            spline = catMullRomSpline(inputPoints_part);

            // 每次的结果都推到result中
            std::vector<Point2d>::const_iterator result_first = spline.begin();
            std::vector<Point2d>::const_iterator result_last  = spline.begin() + spline.size() ;
            std::vector<Point2d> result_part(result_first, result_last);
            result.insert(result.end(), result_part.begin(), result_part.end());
                
            // cout<<"1:"<<spline.size()<<endl;
            // cout<<"2:"<<result.size()<<endl;
        }
        
        return result;
    }
    // 返回四个点中，2、3点的插值点
    else if (numPoints == 4){
        Point2d P0, P1, P2, P3;
        P0.x = inputPoints[0].x;
        P0.y = inputPoints[0].y;
        P1.x = inputPoints[1].x;
        P1.y = inputPoints[1].y;
        P2.x = inputPoints[2].x;
        P2.y = inputPoints[2].y;
        P3.x = inputPoints[3].x;
        P3.y = inputPoints[3].y;

        double t0 = 0;
        double t1 = tj(t0, P0, P1);
        double t2 = tj(t1, P1, P2);
        double t3 = tj(t2, P2, P3);

        // 每两个点中间生成100个点
        // double linespace = (t2 - t1)/numSpace;
        // t2-t1 有多少个linespace，就生成多少个点
        double linespace = 2;

        // cout<<linespace<<endl;
        // cout<<t2-t1<<endl;
        
        double t = t1;

        vector<Point2d> C;
        while( t <= t2){
           
            double A1_x = (t1-t)/(t1-t0)*P0.x + (t-t0)/(t1-t0)*P1.x;
            double A1_y = (t1-t)/(t1-t0)*P0.y + (t-t0)/(t1-t0)*P1.y;
            double A2_x = (t2-t)/(t2-t1)*P1.x + (t-t1)/(t2-t1)*P2.x;
            double A2_y = (t2-t)/(t2-t1)*P1.y + (t-t1)/(t2-t1)*P2.y;
            double A3_x = (t3-t)/(t3-t2)*P2.x + (t-t2)/(t3-t2)*P3.x;
            double A3_y = (t3-t)/(t3-t2)*P2.y + (t-t2)/(t3-t2)*P3.y;
            double B1_x = (t2-t)/(t2-t0)*A1_x + (t-t0)/(t2-t0)*A2_x;
            double B1_y = (t2-t)/(t2-t0)*A1_y + (t-t0)/(t2-t0)*A2_y;
            double B2_x = (t3-t)/(t3-t1)*A2_x + (t-t1)/(t3-t1)*A3_x;
            double B2_y = (t3-t)/(t3-t1)*A2_y + (t-t1)/(t3-t1)*A3_y;

            double C_x = (t2-t)/(t2-t1)*B1_x + (t-t1)/(t2-t1)*B2_x;
            double C_y = (t2-t)/(t2-t1)*B1_y + (t-t1)/(t2-t1)*B2_y;
            C_x = floor(C_x);
            C_y = floor(C_y);
   
            C.push_back( Point2d( C_x, C_y ) );
        
            t = t + linespace;
        }
        return C;
    }
    // 如果输入点不足4个，返回一个（0，0）点作为error
    else{
        cout<<"The number of input points must > 4"<<endl;
        vector<Point2d> nopoint;
        nopoint.push_back(Point2d(0,0));
        return nopoint;
    }
}


int main(int argc,char*argv[])  
{    
    vector<Point2d> input;
   
    input.push_back(Point2d(-520, -530));
    input.push_back(Point2d(-510, -520));
    input.push_back(Point2d(50,50));
    input.push_back(Point2d(300,250));
    input.push_back(Point2d(550,200));
    input.push_back(Point2d(800,600));
    input.push_back(Point2d(900,900));

	vector<Point2d> Output = catMullRomSpline(input);
    cout<<"平滑后的点的个数："<<Output.size()<<endl;

    Mat img(2000, 2000, CV_8UC3, Scalar(255, 255, 255));

    for (int i = 0 ; i< input.size(); i++){
        int x = 1000 + input[i].x ;
        int y = 1000 - input[i].y ;
        circle(img, Point2d(x, y),1, Scalar(0, 0, 255), 15, 8);
    }
    for (int i = 0 ; i< Output.size(); i++){
        int x = 1000 + Output[i].x ;
        int y = 1000 - Output[i].y ;
        circle(img, Point2d(x, y),1, Scalar(0, 255, 0), 8, 8);
    }
    imshow("line fitting", img);
    waitKey(0);
	return 0;
}  
