/*  */#include "agribot_vs.h"

using namespace cv;
using namespace std;
using namespace Eigen;

using std::cerr;
using cv::Mat;

namespace agribot_vs{

  // calss constructor
  AgribotVS::AgribotVS(){

    CenterLine.resize(1, 0);
    CenterLine[0][0] = 1280 / 2;
    CenterLine[0][1] = 0;
    CenterLine[0][2] = 1280 / 2;
    CenterLine[0][3] = 720;

    RobotPose.resize(6, 0);
    RobotLinearVelocities.resize(3, 0);
    RobotAngularVelocities.resize(3, 0);
    RotationVec.resize(3, 0);
    TransVec.resize(3, 0);

    minp_cnt=0;

    id = 0;

    navigation_dir = 1;


    F_des << 0,0,0;
    F << 0,0,0;
  }

  // calss deconstructor
  AgribotVS::~AgribotVS(){
  };

  bool AgribotVS::ReadConfig_run(ros::NodeHandle& nodeHandle_) {

    nodeHandle_.param("/agribot_vs/min_frame", min_frame, 15);
    
    nodeHandle_.param("/agribot_vs/coef", coef, 60.0);
    

    nodeHandle_.param("/agribot_vs/ex_Xc", ex_Xc, 640);
    nodeHandle_.param("/agribot_vs/ex_Yc", ex_Yc, 360);
    nodeHandle_.param("/agribot_vs/nh_offset", nh_offset, 200);
    nodeHandle_.param("/agribot_vs/nh_L", nh_L, 150);
    nodeHandle_.param("/agribot_vs/nh_H", nh_H, 250);

    nodeHandle_.param("/agribot_vs/minContourSize", minContourSize, 8.0);
    
    nodeHandle_.param("/agribot_vs/div_soft", div_soft, 15.0);
    
    nodeHandle_.param("/agribot_vs/cnt_off", cnt_off, 80);

    nodeHandle_.param("/agribot_vs/mode", mode, 1);
    nodeHandle_.param("/agribot_vs/w_control_mode", w_control_mode, 1);

    nodeHandle_.param("/agribot_vs/mask_tune", mask_tune, false);
    nodeHandle_.param("/agribot_vs/single_camera_mode", single_camera_mode, false);

    nodeHandle_.param("/agribot_vs/z_min", z_min, 0.15);

    nodeHandle_.param("/agribot_vs/w_min", w_min, 0.015);
    nodeHandle_.param("/agribot_vs/w_max", w_max, 0.15);
    nodeHandle_.param("/agribot_vs/del_t", del_t, 0.1);
    nodeHandle_.param("/agribot_vs/vf_des", vf_des, 0.05);
    nodeHandle_.param("/agribot_vs/vb_des", vb_des, 0.05);

    nodeHandle_.param("/agribot_vs/ty", ty, 0.1);
    nodeHandle_.param("/agribot_vs/tz", tz, 1.034);

    nodeHandle_.param("/agribot_vs/rho_f", rho_f, -70.0);
    nodeHandle_.param("/agribot_vs/rho_b", rho_b, -40.0);
    rho_b *=DEG2RAD;
    rho_f *=DEG2RAD;

    nodeHandle_.param("/agribot_vs/cam_num", cam_num, 1);

    nodeHandle_.param("/agribot_vs/lambda_x_1", lambda_x_1, 1.0);
    nodeHandle_.param("/agribot_vs/lambda_w_1", lambda_w_1, 1.0);

    nodeHandle_.param("/agribot_vs/lambda_x_2", lambda_x_2, 1.0);
    nodeHandle_.param("/agribot_vs/lambda_w_2", lambda_w_2, 1.0);

    nodeHandle_.param("/agribot_vs/lambda_x_3", lambda_x_3, 1.0);
    nodeHandle_.param("/agribot_vs/lambda_w_3", lambda_w_3, 1.0);

    nodeHandle_.param("/agribot_vs/lambda_x_4", lambda_x_4, 1.0);
    nodeHandle_.param("/agribot_vs/lambda_w_4", lambda_w_4, 1.0);

    nodeHandle_.param("/agribot_vs/height", height, 720);
    nodeHandle_.param("/agribot_vs/width", width, 1280);

    nodeHandle_.param("/agribot_vs/drive_forward", drive_forward, true);
    nodeHandle_.param("/agribot_vs/turning_mode", turning_mode, false);
    nodeHandle_.param("/agribot_vs/steering_dir", steering_dir, 1);
    nodeHandle_.param("/agribot_vs/driving_dir", driving_dir, 1);

    nodeHandle_.param("/agribot_vs/min_points_switch", min_points_switch, 15);
    
    nodeHandle_.param("/agribot_vs/publish_cmd_vel", publish_cmd_vel, true);
    nodeHandle_.param("/agribot_vs/publish_linear_vel", publish_linear_vel, true);

    nodeHandle_.param("/agribot_vs/max_Hue", max_Hue, 80);
    nodeHandle_.param("/agribot_vs/min_Hue", min_Hue, 10);

    nodeHandle_.param("/agribot_vs/max_Saturation", max_Saturation, 255);
    nodeHandle_.param("/agribot_vs/min_Saturation", min_Saturation, 100);

    nodeHandle_.param("/agribot_vs/max_Value", max_Value, 255);
    nodeHandle_.param("/agribot_vs/min_Value", min_Value, 100);

    nodeHandle_.param("/agribot_vs/center_max_off", center_max_off, 100);
    nodeHandle_.param("/agribot_vs/center_min_off", center_min_off, 100);

    nodeHandle_.param("/agribot_vs/LineDiffOffset", LineDiffOffset, 0.1);

    nodeHandle_.param("/agribot_vs/max_vel_lin_", max_vel_lin_, 0.4);
    nodeHandle_.param("/agribot_vs/min_vel_lin_", min_vel_lin_, 0.01);
    nodeHandle_.param("/agribot_vs/max_incr_lin_", max_incr_lin_, 0.1);
    nodeHandle_.param("/agribot_vs/k_p_lin_", k_p_lin_, 0.5);
    nodeHandle_.param("/agribot_vs/k_i_lin_", k_i_lin_, 0.0);
    nodeHandle_.param("/agribot_vs/k_d_lin_", k_d_lin_, 0.1);

    nodeHandle_.param("/agribot_vs/max_vel_ang_", max_vel_ang_, 0.05);
    nodeHandle_.param("/agribot_vs/min_vel_ang_", min_vel_ang_, 0.005);
    nodeHandle_.param("/agribot_vs/max_incr_ang_", max_incr_ang_, 0.1);
    nodeHandle_.param("/agribot_vs/k_p_ang_", k_p_ang_, 0.1);
    nodeHandle_.param("/agribot_vs/k_i_ang_", k_i_ang_, 0.0);
    nodeHandle_.param("/agribot_vs/k_d_ang_", k_d_ang_, 0.1);

    nodeHandle_.param("/agribot_vs/Scale", Scale, 0.5);

    nodeHandle_.param("/agribot_vs/FilterQuieSize", FilterQuieSize, 10);

    if( !nodeHandle_.getParam("/agribot_vs/x_poses", x_poses) ) ROS_ERROR("Failed to get parameter from server.");
    if( !nodeHandle_.getParam("/agribot_vs/y_poses", y_poses) ) ROS_ERROR("Failed to get parameter from server.");
    if( !nodeHandle_.getParam("/agribot_vs/z_poses", z_poses) ) ROS_ERROR("Failed to get parameter from server.");

    nodeHandle_.param("/agribot_vs/LineFitting_method", LineFitting_method, 0);
    nodeHandle_.param("/agribot_vs/ControllerType", ControllerType, 0);

    cout << "Run parameters loading ..." << endl;

    

    return true;
  }

  vector<vector<Point>> AgribotVS::CropRowFeatures(Mat& img) {
    // // convert to HSV color space
    cv::Mat hsvImage;
    cv::cvtColor(img, hsvImage, CV_BGR2HSV);
    //cv::imshow("HSV image", hsvImage);
    // split the channels
    std::vector<cv::Mat> hsvChannels;
    cv::split(hsvImage, hsvChannels);

    // is the color within the lower hue range?
    cv::Mat hueMask;
    cv::Mat hueImg = hsvChannels[0];
    cv::inRange(hueImg, min_Hue, max_Hue, hueMask);
    cv::Mat saturationMask;
    cv::Mat saturationImg = hsvChannels[1];
    cv::inRange(saturationImg, min_Saturation, max_Saturation, saturationMask);
    cv::Mat valueMask;
    cv::Mat valueImg = hsvChannels[2];
    cv::inRange(valueImg, min_Value, max_Value, valueMask);

    Mat Comb_HSV;
    hconcat(hueMask,saturationMask,Comb_HSV);
    hconcat(Comb_HSV,valueMask ,Comb_HSV);
    cv::resize(Comb_HSV, Comb_HSV, cv::Size(), Scale, Scale);
    // cv::imshow("HSV image", Comb_HSV);

    hueMask = (hueMask & saturationMask) & valueMask;
    //cv::imshow("Desired color", hueMask);
    // now perform the line detection
    
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);

    // finds cluster/contour in the image
    findContours(hueMask, contours, hierarchy, CV_RETR_TREE,
                CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Draw contours
    img_contour = Mat::zeros(hueMask.size(), CV_8UC3);
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    for(size_t i = 0; i < contours.size(); i++){
      drawContours(img_contour, contours, i, color, 1, 8, hierarchy, 0, Point());
    }

    if(mask_tune){
      cv::resize(img_contour, img_contour, cv::Size(), Scale, Scale);
      imshow("Contours", img_contour);
      waitKey(1);
    }

    // imshow("img_contour", img_contour);
    // waitKey(1);
    return contours;
  }

  vector<Point2f> AgribotVS::getContureCenters(Mat& img, vector<vector<Point>>& contours){
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point>> contours_poly(contours.size());
    vector<Point2f> center(contours.size());
    vector<float> radius(contours.size());

    // find enclosing Polygon whcih fits arround the contures 
    for (size_t i = 0; i < contours.size(); i++) {
      // for(size_t j = 0; j < contours[i].size(); j++)
      // {
      //   center.push_back(Point2f(contours[i][j].x,contours[i][j].y));
      // }
      
      approxPolyDP(Mat(contours[i]), contours_poly[i], 2, true);
      minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
      cv::circle(img, Point(center[i].x, center[i].y),3, Scalar(51, 204, 51),CV_FILLED, 8,0);
    }
    return center;
  }

  vector<Point2f> AgribotVS::filterContures(Mat& img, vector<vector<Point>>& contours){
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point>> contours_poly(contours.size());
    vector<Point2f> center(contours.size());
    vector<float> radius(contours.size());
    vector<Point2f> Filtered_Centers;
    RNG rng(12345);
    // find enclosing Polygon whcih fits arround the contures 
    for (size_t i = 0; i < contours.size(); i++) {
      approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
      minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
      if (center[i].x >= (img.cols/2) - center_min_off && 
          center[i].x <= (img.cols/2) + center_max_off && 
          radius[i] >= this->minContourSize) {
        Filtered_Centers.push_back(center[i]);
        //Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
        //cv::circle(img, Point(center[i].x, center[i].y),15, color, 3,0); //CV_FILLED  Scalar(100,50,100)
      }

    }
    return Filtered_Centers;
  }

  vector<Vec4i> AgribotVS::FitLineOnContures(Mat& img, vector<Point2f>& ContourCenters){

    
    Point2f P1;
    Point2f P2;
    vector<Vec4i> AvgLine;
    AvgLine.resize(1);
   
    if(ContourCenters.size() > 0 ){
      Vec4f linefit;
      cv::fitLine(ContourCenters,linefit,CV_DIST_L2,0,0.01,0.01);
      // float vx = linefit[0];
      // float vy = linefit[1];
      // x = linefit[2];
      // y = linefit[3];
      // int lefty = round((-x * vy / vx) + y);
      // int righty = round(((img.cols - x) * vy / vx) + y);
      // line[0] = img.cols - 1;
      // line[1] = righty;
      // line[2] = 0;
      // line[3] = lefty;

      P1.x = linefit[0] + linefit[2];
      P1.y = linefit[1] + linefit[3];
      P2.x = linefit[2];
      P2.y = linefit[3];

      Vector3f top_left(0,0,1);
      Vector3f top_right(width,0,1);
      Vector3f bottom_left(0,height,1);
      Vector3f bottom_right(width,height,1);

      // compute image border lines
      Vector3f l_ib = bottom_left.cross(bottom_right); 
      Vector3f l_it = top_left.cross(top_right);
      Vector3f l_il = top_left.cross(bottom_left);
      Vector3f l_ir = top_right.cross(bottom_right);

      // compute line PQ
      Vector3f P_h(P1.x,P1.y,1);
      Vector3f Q_h(P2.x,P2.y,1);
      Vector3f l = P_h.cross(Q_h);

      // compute intersections with all four lines
      Vector2f R_t = hom2euc(l.cross(l_it));
      Vector2f R_l = hom2euc(l.cross(l_il));
      Vector2f R_b = hom2euc(l.cross(l_ib)); 
      Vector2f R_r = hom2euc(l.cross(l_ir));

      // compute points within the image 
      MatrixXf R(4,2);
      R << R_t(0), R_t(1),
          R_l(0), R_l(1),
          R_b(0), R_b(1),
          R_r(0), R_r(1);

      MatrixXf R_out(2,2);
      R_out = is_in_image_point(R);

      // if(Vectlines.size() >= (uint)FilterQuieSize){
      // Vectlines.erase(Vectlines.begin());
      // }
      // Vectlines.push_back(line);

      // for (uint i = 0; i < Vectlines.size(); i++)
      // {
      //   AvgLine[0][0] += Vectlines[i][0];
      //   AvgLine[0][1] += Vectlines[i][1];
      //   AvgLine[0][2] += Vectlines[i][2];
      //   AvgLine[0][3] += Vectlines[i][3];
      // }
      // AvgLine[0][0] /= FilterQuieSize;
      // AvgLine[0][1] /= FilterQuieSize;
      // AvgLine[0][2] /= FilterQuieSize;
      // AvgLine[0][3] /= FilterQuieSize;

      // Vec4i l = AvgLine[0];
      //cout << R_out(0,0) << " "<< R_out(0,1) << " "<<  R_out(1,0) << " "<< R_out(1,1) << " " << endl;
      cv::line(img, Point(R_out(0,0), R_out(0,1)),Point(R_out(1,0), R_out(1,1)), Scalar(0, 0, 255), 1, CV_AA);
      AvgLine[0][0] =R_out(0,0);
      AvgLine[0][1] =R_out(0,1);
      AvgLine[0][2] =R_out(1,0);
      AvgLine[0][3] =R_out(1,1);
    }
    return AvgLine;
  }

  vector<Vec4i> AgribotVS::HouphLineOnImage(Mat& img, vector<Point2f>& ContourCenters){

    cv::Vec4i line;
    vector<Vec4i> AvgLine;
    AvgLine.resize(1);
    vector<Vec4i> lines;
    Mat dst, thr;
    
    //Canny(img, dst, 50, 200, 3);
    Mat tmp = img;
    //Mat mask = extract_vegitation_mask(tmp);
    // imshow("vegetation mask", mask);
    // cv::waitKey(1);
    // cv::HoughLinesP(mask, lines, 1, CV_PI/180, 100,100,400 );
    // cout << lines.size() << endl;
    // for( size_t i = 0; i < lines.size(); i++ ){
    //   Vec4i l = lines[i];
    //   cv::line( tmp, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
    // }
    // imshow("tmp", tmp);
    // cv::waitKey(1);
    // if(Vectlines.size() >= (uint)FilterQuieSize){
    //   Vectlines.erase(Vectlines.begin());
    //   }
    //   Vectlines.push_back(line);

    //   for (uint i = 0; i < Vectlines.size(); i++)
    //   {
    //     AvgLine[0][0] += Vectlines[i][0];
    //     AvgLine[0][1] += Vectlines[i][1];
    //     AvgLine[0][2] += Vectlines[i][2];
    //     AvgLine[0][3] += Vectlines[i][3];
    //   }
    //   AvgLine[0][0] /= FilterQuieSize;
    //   AvgLine[0][1] /= FilterQuieSize;
    //   AvgLine[0][2] /= FilterQuieSize;
    //   AvgLine[0][3] /= FilterQuieSize;

    //   Vec4i l = AvgLine[0];
    //   cv::line(img, Point(l[0], l[1]),Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
    return AvgLine;
  }

  //*************************************************************************************************
  void AgribotVS::Controller(camera& I_primary, camera& I_secondary){
    // id = 1 => Row controller
    // id = 2 => Column controller
    // input feature

    float X = F(0); 
    float Y = F(1); 
    float Theta = F(2);

    float v = 0;
    if( driving_dir ==1){
      v = driving_dir*vf_des;
    }else{
      v = driving_dir*vb_des;
    }
    // compute interaction matrix

    MatrixXf lambda(2,1);
    if(driving_dir == 1){
      if(steering_dir == 1){ // mode 1
        lambda << lambda_x_1,lambda_w_1;
        rho = rho_f;
        mode = 1;
        //ty = 0.0;
      }else{                 // mode 2
        lambda << lambda_x_2,lambda_w_2;
        rho = rho_b;
        mode = 2;
        //ty = 1.08;
      }
    }else{
      if(steering_dir == -1){ // mode 3
        lambda << lambda_x_3,lambda_w_3;
        rho = rho_b;
        mode = 3;
        //ty = 1.08;
      }else{                  // mode 4
        lambda << lambda_x_4,lambda_w_4;
        rho = rho_f;
        mode = 4;
        //ty = 0.0;
      }
    }

    MatrixXf Ls(3,6);
      Ls << -(sin(rho)+Y*cos(rho))/tz, 0, X*(sin(rho)+Y*cos(rho))/tz, X*Y, -1-pow(X,2),  Y,
            0,   -(sin(rho)+Y*cos(rho))/tz, Y*(sin(rho)+Y*cos(rho))/tz, 1+pow(Y,2), -X*Y, -X,
            cos(rho)*pow(cos(Theta),2)/tz, cos(rho)*cos(Theta)*sin(Theta)/tz, -(cos(rho)*cos(Theta)*(Y*sin(Theta) + X*cos(Theta)))/tz, -(Y*sin(Theta) + X*cos(Theta))*cos(Theta), -(Y*sin(Theta) + X*cos(Theta))*sin(Theta), -1;

    // compute tranformation between robot to camera frame
    MatrixXf c(2,6); 
    if( cam_num == 1){
      c << 0, -sin(rho), cos(rho), 0, 0, 0,
          -ty, 0, 0, 0, -cos(rho ), -sin(rho);
    }else{
      c << 0, sin(rho), -cos(rho), 0, 0, 0,
        -ty, 0, 0, 0, cos(rho), sin(rho);
    }

    MatrixXf cTR(6,2);
      cTR = c.transpose();

    MatrixXf Tv(6,1);
      Tv = cTR.col(0);
    
    MatrixXf Tw(6,1);
      Tw = cTR.col(1);

    // compute Jacobian
    MatrixXf Jv(2,6);
      Jv << Ls.row(id),
            Ls.row(2);
      Jv = Jv*Tv;

    MatrixXf Jw(2,6);
      Jw << Ls.row(id),
            Ls.row(2);
      Jw = Jw*Tw;

    // // compute control law
    Vector2f err((F[id] - F_des[id]), wrapToPi(F[2] - F_des[2]));

    // set weights
    MatrixXf tmp_lambda(2,1);
      tmp_lambda << lambda(0)*err(0),
                    lambda(1)*err(1);

    // compute control
    MatrixXf Jw_pinv(6,2);
    Jw_pinv = Jw.completeOrthogonalDecomposition().pseudoInverse();

    MatrixXf w = -Jw_pinv*(tmp_lambda + Jv*v);

    //Limit w
    if(w_control_mode == 0){
      w(0,0) = copysign(min(abs(w(0,0)),
        (float)w_max), w(0,0));
    }else if(w_control_mode == 1){
      double w_n = (err(0)/div_soft)-5;
      double w_p = (err(0)/div_soft)+5;
      if(v > 0){
        if(w(0,0) > 0){
          w(0,0) *= exp(w_n)/(1+exp(w_n));
        }else{
          w(0,0) *= 1- (exp(w_p)/(1+exp(w_p)));
        }
      }else{
        if(w(0,0) > 0){
          w(0,0) *= exp(w_n)/(1+exp(w_n));
        }else{
          w(0,0) *= 1- (exp(w_p)/(1+exp(w_p)));
        }
      }
    }else{
      cout << w_control_mode << endl;
      double w_n = (err(0)/div_soft)-5;
      double w_p = (err(0)/div_soft)+5;
      if(v > 0){
        if(w(0,0) < 0){
          w(0,0) *= (exp(w_n)/(1+exp(w_n)));
        }else{
          w(0,0) *= (1 - (exp(w_p)/(1+exp(w_p))));
        }
      }else{
        if(w(0,0) > 0){
          w(0,0) *= (exp(w_n)/(1+exp(w_n)));
        }else{
          w(0,0) *= (1 - (exp(w_p)/(1+exp(w_p))));
        }
      }
      
      w(0,0) = copysign(min(fabs(w(0,0)),(float)w_max), w(0,0));
      w(0,0) = copysign(max(fabs(w(0,0)),(float)w_min), w(0,0));
    }

    if(abs(v) < 0.05 ||  (I_primary.nh_points.size() < 5  && I_secondary.nh_points.size() <5)){
      VelocityMsg.angular.z = 0;
    }else{
      if(w(0,0) > 0.3) w(0,0) = 0.3;
      VelocityMsg.angular.z = steering_dir * w(0,0);
    }

    
    if(abs(VelocityMsg.angular.z) < z_min || mode == 4 || mode==2)VelocityMsg.angular.z=0.0;
    VelocityMsg.linear.x  = v;

    VSMsg.err_x=abs(err(0));
    VSMsg.err_theta=abs(err(1));
 
    //cout << " ty: " << ty << " tz:" << tz << endl;
    //cout << " F: " << F(0) << " " << F(1) << " " << F(2) << endl;
    //cout << " F_des: " << F_des(0) << " " << F_des(1) << " "  << F_des(2) << endl;
    //cout << " lin-x: " << VelocityMsg.linear.x << " ang-z:" << VelocityMsg.angular.z << endl; 
  }

  void AgribotVS::compute_feature_point(camera& I){

    if(drive_forward){
      P.x = I.lines[0][0];
      P.y = I.lines[0][1];
      Q.x = I.lines[0][2];
      Q.y = I.lines[0][3];
    }else{
      P.x = I.lines[0][2];
      P.y = I.lines[0][3];
      Q.x = I.lines[0][0];
      Q.y = I.lines[0][1];
    }

    I.nh_ex = Compute_nh_ex(I);
    I.nh.Xc = (I.nh_ex[0].x + I.nh_ex[1].x)/2;
    I.nh.Yc = (I.nh_ex[0].y + I.nh_ex[1].y)/2;

    // computes intersection side
    int side_image = compute_intersection(P, Q);
    cv::circle(I.image, Point(P.x, P.y),8, Scalar(0,0,255),CV_FILLED, 12,0);
    cv::circle(I.image, Point(Q.x, Q.y),8, Scalar(0,255,255),CV_FILLED, 12,0);

    // compute Theta
    float Theta = compute_Theta(P,Q);
    
    // compute F
    Point2f _F = camera2image(P);
    F << _F.x,
         _F.y,
         Theta;
    F_des <<  0,
              height/2,
              0;
    id = 0;

    // switch(side_image){
    //   case 0:{
    //       // Case 1
    //       id = 0;
    //       if(Theta > 0){
    //           // 1.1
    //           F_des <<  -width/2,
    //                     -height/2,
    //                      M_PI/2 + M_PI/8;
    //       }
    //       else{
    //         // 1.2
    //         F_des << width/2, 
    //                 -height/2, 
    //                 wrapToPi(M_PI+M_PI/8);
    //       }
    //   break;}      
    //   case 1:{
    //       // Case 2
    //       F_des << -width/2,
    //                 height/2, 
    //                 M_PI/4;
    //       id = 1;
    //   break;}
    //   case 2:{
    //       // Case 3
    //       F_des << 0,
    //               height/2,
    //               0;
    //       id = 0;
    //   break;}
    //   case 3:{
    //       // Case 4
    //       F_des << width/2, 
    //                height/2, 
    //               wrapToPi(M_PI+M_PI/2+M_PI/4);
    //       id = 1;
    //   break;}

    //}
  }

  void AgribotVS::switching_controller(camera& I_primary, camera& I_secondary, int min_points){
    // check camera image and select camera

    is_in_neigbourhood(I_primary); 

    double avg_nh_points_y = 0.0;
    for (int i = 0; i < I_primary.nh_points.size(); ++i){
      avg_nh_points_y += I_primary.nh_points[i].y;
    }
    avg_nh_points_y /= I_primary.nh_points.size();

    // double avg_nh_points_x = 0.0;
    // for (int i = 0; i < I_primary.nh_points.size(); ++i){
    //   avg_nh_points_x += I_primary.nh_points[i].x;
    // }
    // avg_nh_points_y /= I_primary.nh_points.size();

    double avg_points_y =0.0;
    for (int i = 0; i < I_primary.points.size(); ++i){
      avg_points_y += I_primary.points[i].y;
    }
    avg_points_y /= I_primary.points.size();

    // if(I_primary.nh_points.size() <= min_points|| 
    //     (I_primary.nh.Yc < (1/8)*height && avg_points_y < (1/3)*height) ||
    //     (I_primary.nh.Yc > (7/8)*height && avg_points_y < (2/3)*height)){
    if(I_primary.nh_points.size() < min_points){
      minp_cnt++;
    }else{
      minp_cnt = 0;
    }
    
    // cout
    // << " nh _1: " <<I_primary.nh_points.size() << "\n"
    // << " img _1: " <<I_primary.points.size() << "\n"
    // << " img _2: " <<I_secondary.points.size() << "\n"
    // << " avg_nh_points_y: " << avg_nh_points_y << "\n"
    // << " avg_points_y: " << avg_points_y << "\n"
    // << endl; 

    // if(I_primary.nh_points.size() < min_points && minp_cnt >=15){
    if(I_primary.nh_points.size() == 0 && minp_cnt >=min_frame ||  
      (avg_nh_points_y < (double)coef && avg_points_y < (double)coef*2) || 
      (avg_nh_points_y > (double)height-coef && avg_points_y > (double)height-coef*2)){
      minp_cnt=0;
      // No points visible in primary camera
      cout << "I_primary doesn't see anything !!!! id: " << cam_num << endl;
      // initialize_neigbourhood(I_secondary);
      // is_in_neigbourhood(I_secondary);
      if(I_secondary.points.size() < min_points){
        cout << "turning_mode: " << turning_mode << endl;
          // No points visible in both cameras
          if(turning_mode){
            // 2->3 and 4->1
            mode++;
            if(mode == 5)mode=1;
            cout << "TURNING..." << endl;
            // switch steering direction
            driving_dir = -driving_dir;
            // switch behavior cameras
            drive_forward = true;
            turning_mode  = false;
            cout << "turning mode OFF" << endl;
            // shift the neigbourhood
            //steering_dir = -steering_dir;
            shift_neighbourhood(I_primary, steering_dir);
            is_in_neigbourhood(I_primary);  

          }
      }else{
          mode++;
          // 1->2 and 3->4
          cout << "SWITCHING CAMERAS" << endl;
          switch_cameras(cam_num);
          initialize_neigbourhood(I_secondary);
          initialize_neigbourhood(I_primary);
          is_in_neigbourhood(I_primary);   
          turning_mode = true;
          drive_forward = false;
          steering_dir = -steering_dir;
          cout << "turning mode ON" << endl;
      }
    }else{
        cout  << 
        "mode: " << mode << 
        ", cam: " <<  cam_num << 
        ", df: " << drive_forward << 
        ", sd: " << steering_dir << 
        ", dd: " << driving_dir << 
        ", nh_p: " << I_primary.nh_points.size() <<
         endl;
    }
  }

  int AgribotVS::compute_intersection(Point2f& P, Point2f& Q){
    // computes side given the points P, Q - side = 1 (Top) - side = 2 (Left) - side = 3 (Bottom) - side = 4 (Right)

    Vector3f top_left(0,0,1);
    Vector3f top_right(width,0,1);
    Vector3f bottom_left(0,height,1);
    Vector3f bottom_right(width,height,1);

    // compute image border lines
    Vector3f l_ib = bottom_left.cross(bottom_right); 
    Vector3f l_it = top_left.cross(top_right);
    Vector3f l_il = top_left.cross(bottom_left);
    Vector3f l_ir = top_right.cross(bottom_right);

    // compute line PQ
    Vector3f P_h(P.x,P.y,1);
    Vector3f Q_h(Q.x,Q.y,1);
    Vector3f l =  P_h.cross(Q_h);

    // compute intersections with all four lines
    Vector2f R_t = hom2euc(l.cross(l_it));
    Vector2f R_l = hom2euc(l.cross(l_il));
    Vector2f R_b = hom2euc(l.cross(l_ib)); 
    Vector2f R_r = hom2euc(l.cross(l_ir));

    // compute points within the image 
    MatrixXf R(4,2);
    R << R_t(0), R_t(1),
         R_l(0), R_l(1),
         R_b(0), R_b(1),
         R_r(0), R_r(1);
    Vector4i in = is_in_image(R);
    
    int ind_min = 0;
    int ind_max = 0;
    int tmp_min_y = 10000;
    int tmp_max_y = 0;
    int cnt = 0;
    for(int i = 0; i < 4; i++){
      if(in(i) == 1){
        if(R(i,1) < tmp_min_y){
          tmp_min_y = R(i,1);
          ind_min = i;
        }
        if(R(i,1) > tmp_max_y){
          tmp_max_y = R(i,1);
          ind_max = i;
        }
      }
    }
    
    Q.x = R(ind_min,0);
    Q.y = R(ind_min,1);
    P.x = R(ind_max,0);
    P.y = R(ind_max,1);

    return ind_max;
  }

  void AgribotVS::compute_intersection_old(Point2f& P, Point2f& Q){
    // computes side given the points P, Q - side = 1 (Top) - side = 2 (Left) - side = 3 (Bottom) - side = 4 (Right)

    Vector3f origin_h(0,0,1);
    Vector3f W_h(width,0,1);
    Vector3f H_h(0,height,1);
    Vector3f WH_h(width,height,1);

    // compute image border lines
    Vector3f l_ib = origin_h.cross(W_h); 
    Vector3f l_it = H_h.cross(WH_h);
    Vector3f l_il = origin_h.cross(H_h);
    Vector3f l_ir = W_h.cross(WH_h);

    // compute line PQ
    Vector3f P_h(P.x,P.y,1);
    Vector3f Q_h(Q.x,Q.y,1);
    Vector3f l =  P_h.cross(Q_h);

    // compute intersections with all four lines
    Vector2f R_t = hom2euc(l.cross(l_it));
    Vector2f R_l = hom2euc(l.cross(l_il));
    Vector2f R_b = hom2euc(l.cross(l_ib)); 
    Vector2f R_r = hom2euc(l.cross(l_ir));

    // compute points within the image 
    MatrixXf R(4,2);
    R << R_t(0), R_t(1),
         R_l(0), R_l(1),
         R_b(0), R_b(1),
         R_r(0), R_r(1);
    Vector4i in = is_in_image(R);
    
    // // intersection points
    MatrixXf S(2,2);
    S << 0,0,
         0,0;

    VectorXi Sind(2);
         Sind << 0,0;

    // Sind = find(in); -> matlab 
    int cnt = 0;
    for(int i = 0; i < 4; i++){
      if(in(i) == 1){
        S.row(cnt) = R.row(i);
        Sind(cnt) = cnt; 
        cnt++;
      }
    }

    // angle PQ
    float Y = Q.y-P.y;
    float X = Q.x-P.x;
    float phi = wrapToPi(atan2(Y,X)+M_PI);
    
    //distance to P 
    MatrixXf _P(2,1);
    _P << P.x,
          P.y;
    Vector2f D = dist(S,_P);

    // points along the line
    MatrixXf S_(2,2);
    S_ << P.x + D(0) * cos(phi), P.y + D(0) * sin(phi),
          P.x + D(1) * cos(phi), P.y + D(1) * sin(phi); 

    // // distance to original points
    Vector2f DS = dist(S,S_);

    // matching points
    // [~, minds] = min(DS);
    int minds = 0;
    if(DS(0,0) < DS(1,0)) minds = 0;
    else minds = 1;
    side = Sind(minds);
    // Intersection of P with image boundaries
    P.x = S(minds,0);
    P.y = S(minds,1);

    // Intersection of Q with image boundaries
    // [~, maxds] = max(DS);
    int maxds = 0;
    if(DS(0,0) < DS(1,0)) maxds = 1;
    Q.x = S(maxds,0);
    Q.y = S(maxds,1);
  }

  Vector2f AgribotVS::hom2euc(Vector3f Mat){
    Vector2f _Mat;
    _Mat <<  Mat(0,0)/Mat(2,0),
            Mat(1,0)/Mat(2,0);
    return _Mat;
  }

  VectorXi AgribotVS::find(Eigen::Vector4i A){
    VectorXi idxs;
    for(int i=0; i<A.size(); i++)
        if(A(i))
            idxs << i;
    return idxs;
  }

  Vector4i AgribotVS::is_in_image(MatrixXf R){
    Vector4i _In(0, 0, 0, 0);
    for(int i = 0; i < 4; i++){
      if(R(i,0) >= 0 && R(i,0) <= width && R(i,1) >= 0 && R(i,1) <= height){
        _In(i) = 1;
      }
    }
    return _In;
  }

  MatrixXf AgribotVS::is_in_image_point(MatrixXf R){
    MatrixXf out_p(2,2);
    int cnt = 0;
    for(int i = 0; i < 4; i++){
      if(R(i,0) >= 0 && R(i,0) <= width && R(i,1) >= 0 && R(i,1) <= height){
        out_p(cnt,0) = R(i,0);
        out_p(cnt,1) = R(i,1);
        cnt++;
      }
    }
    return out_p;
  }

  Vector2f AgribotVS::dist(MatrixXf& A, MatrixXf& B){
    Vector2f dis;
    Vector2f tmp;
    //D = sqrt(sum((A-B).^2,2));
    for(size_t i = 0; i < A.cols(); i++){
        tmp << sqrt(pow((A(0,i) - B(0,0)),2)),
              sqrt(pow((A(1,i) - B(0,0)),2));
        dis << tmp;
    }
    return dis;
  }

  void AgribotVS::switch_cameras(int& cam_primary){
    if(cam_primary == 1)
        cam_primary=2;
    else{
       cam_primary =1;
    }
  }

  Point2f AgribotVS::camera2image(Point2f& xc){
    Point2f xi;
    xi.x =  xc.x - width/2;
    xi.y =  xc.y - height/2;
    return xi;
  }

  Point2f AgribotVS::camera2origin(Point2f& xc){
    Point2f xi;
    xi.x =  xc.x;
    xi.y =  height - xc.y;
    return xi;
  }

  Point2f AgribotVS::origin2camera(Point2f& xc){
    Point2f xi;
    xi.x =  xc.x;
    xi.y =  height - xc.y;
    return xi;
  }

  Point2f AgribotVS::origin2image(Point2f& xc){
    Point2f xi,xo;
    xi = origin2camera(xc);
    xo = camera2origin(xi);
    return xo;
  }

  Point2f AgribotVS::image2camera(Point2f& xc){
    Point2f xi;
    xi.x =  xc.x + width/2;
    xi.y =  xc.y + height/2;
    return xi;
  }

  Point2f AgribotVS::image2origin(Point2f& xc){
    Point2f xi,xo;
    xi = image2camera(xc);
    xo = camera2origin(xi);
    return xo;
  }

  void AgribotVS::initialize_neigbourhood(camera& I){
    cout << ex_Xc << " " << ex_Yc << endl;
    I.nh.Xc = ex_Xc;
    I.nh.Yc = ex_Yc;
    I.nh.L = nh_L;
    I.nh.H = nh_H;
    I.nh.offset = nh_offset;
    I.nh.shift = navigation_dir * I.nh.offset;
    cout << "***********initialize_neigbourhood***********" << endl;
  }

  void AgribotVS::shift_neighbourhood(camera& I, int shift_dir){
    // To Do:
    // compute using proper formula later
    int dX = shift_dir*I.nh.shift;
    int dY = 0;
    cout << "###########shift_neighbourhood############" << endl;
    // new neighbourhood to select row
    cout << I.nh.Xc  << " " << I.nh.Xc + dX  << " " << I.nh.shift << "  " << shift_dir << endl;
    I.nh.Xc = I.nh.Xc + dX;
  }

  void AgribotVS::is_in_neigbourhood(camera& I){
    I.nh_points.clear();
    for(size_t i = 0; i < I.points.size(); i++){
      if(I.points[i].x > I.nh.Xc - I.nh.L/2 && I.points[i].x < I.nh.Xc + I.nh.L/2 
      && I.points[i].y > I.nh.Yc -I.nh.H/2 && I.points[i].y < I.nh.Yc + I.nh.H/2){
        I.nh_points.push_back(I.points[i]);
      }
    }
  }

  void AgribotVS::draw_neighbourhood(camera& I){
    Point2f P(I.nh.Xc,I.nh.Yc);
    int X = P.x - I.nh.L/2;
    int Y = P.y - I.nh.H/2;
    Rect RectangleToDraw3(X, Y, I.nh.L, I.nh.H);
    rectangle(I.image, RectangleToDraw3, Scalar(255, 204, 102), 3, 8, 0);
  }

  void AgribotVS::draw_features(camera& I, Vector3f Feature, cv::Scalar color){
    
    Point2f P_1(Feature(0),Feature(1));
    P_1 = image2camera(P_1);
    Point2f P_2(P_1.x +  100*cos(Feature(2) - M_PI/2),
                P_1.y + 100*sin(Feature(2) - M_PI/2));
    arrowedLine(I.image, P_1, P_2, color,3);
  }

  vector<Point2f> AgribotVS::Compute_nh_ex(camera& I){
    vector<Point2f> nh_ex;
    nh_ex.resize(2);
    int tmp_min_y = height;
    int tmp_max_y = 0;

    for(size_t i = 0; i < I.nh_points.size(); i++)
    {
      if(I.nh_points[i].y < tmp_min_y){
        tmp_min_y = I.nh_points[i].y;
        nh_ex[0] = I.nh_points[i];
      }
      if(I.nh_points[i].y > tmp_max_y){
        tmp_max_y = I.nh_points[i].y;
        nh_ex[1] = I.nh_points[i];
      }
    }
    return nh_ex;
  }

  float AgribotVS::compute_Theta(Point2f& P, Point2f& Q){
    // compute phi
    float Y = -Q.y+P.y;
    float X = Q.x-P.x;
    float phi = atan2(Y,X);

    // compute Theta
    float Theta = wrapToPi(M_PI/2 - phi);
    
    return Theta;
  }

  float AgribotVS::wrapToPi(float angle){
    while(angle < -M_PI && angle > M_PI){
      if(angle > M_PI){
        angle = angle - 2*M_PI;
      }else if(angle < -M_PI){
        angle = angle + 2*M_PI;
      }
    }
      return angle;
  }

  //*************************************************************************************************
  vector<vector<Point>> AgribotVS::extract_vegetation_mask(Mat& image){
    const int MAN_THRESHOLD = -1;
    const int MIN_SEGMENT_SIZE = 50;
    const int ADAPTIVE_NEIGHBOURHOOD = 251;
    const int ADAPTIVE_C = 5;
    preProcessImageData(image);
    Mat red, green, blue;
    splitRgbImage(image, red, green, blue);
    Mat mask, rgb_vi;
    getVegetationMask(red, green, blue, rgb_vi, mask,
                                         MAN_THRESHOLD, MIN_SEGMENT_SIZE);
    //imshow("b_d", mask);
    //waitKey(1);
    int dilation_size =5;
    Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
    /// Apply the dilation operation
    Mat dilation_dst;
    dilate( mask, dilation_dst, element );

    //imshow("a_d", dilation_dst);
    //waitKey(1);
      
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);

    // finds cluster/contour in the image
    findContours(dilation_dst, contours, hierarchy, CV_RETR_TREE,
                CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Draw contours
    Mat img_contour = Mat::zeros(image.size(), CV_8UC3);
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    for(size_t i = 0; i < contours.size(); i++){
      drawContours(img_contour, contours, i, color, 1, 8, hierarchy, 0, Point());
    }
    return contours;
  }

  void AgribotVS::splitRgbImage(const cv::Mat& bgr, cv::Mat& red,
                                       cv::Mat& green, cv::Mat& blue) {
    // split image
    cv::Mat channel[3];
    cv::split(bgr, channel);

    // assingn
    blue = channel[0];
    green = channel[1];
    red = channel[2];
  }

  void AgribotVS::preProcessRGBdata(cv::Mat& red, cv::Mat& green,
                                           cv::Mat& blue) {
    cv::Size kernel(3,3);
    cv::GaussianBlur(red, red, kernel, 5.0);
    cv::GaussianBlur(green, green, kernel, 5.0);
    cv::GaussianBlur(blue, blue, kernel, 5.0);
  }

  void AgribotVS::mergeRgbImage(cv::Mat& bgr, const cv::Mat& red, const cv::Mat& green, const cv::Mat& blue) {
    std::vector<cv::Mat> mixer(3);
    mixer.at(0) = blue;
    mixer.at(1) = green;
    mixer.at(2) = red;

    cv::merge(mixer, bgr);
  }

  void AgribotVS::getVegetationMask(const cv::Mat& red,
                                            const cv::Mat& green,
                                            const cv::Mat& blue, cv::Mat& rgbVi,
                                            cv::Mat& mask,
                                            const int& manThreshold,
                                            const int& minSegmentSize) {
    AgribotVS::calcRGBVI(red, green, blue, rgbVi, true);

    /// get first approximation of vegetation mask
    if (manThreshold < 0)
      cv::threshold(rgbVi, mask, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    else
      cv::threshold(rgbVi, mask, manThreshold, 255, CV_THRESH_BINARY);

    /// morphologiocal operations
    //! structuring element for the morphological opening and closing of the
    // vegetation mask
    static cv::Mat disk4 =
        (cv::Mat_<uchar>(4, 4) << 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0);

    // opening
    cv::erode(mask, mask, disk4, cv::Point(-1, -1), 1);
    cv::dilate(mask, mask, disk4, cv::Point(-1, -1), 1);
    // closing
    cv::dilate(mask, mask, disk4, cv::Point(-1, -1), 1);
    cv::erode(mask, mask, disk4, cv::Point(-1, -1), 1);
    // flip
    cv::flip(mask, mask, -1);
    // opening
    cv::erode(mask, mask, disk4, cv::Point(-1, -1), 1);
    cv::dilate(mask, mask, disk4, cv::Point(-1, -1), 1);
    // closing
    cv::dilate(mask, mask, disk4, cv::Point(-1, -1), 1);
    cv::erode(mask, mask, disk4, cv::Point(-1, -1), 1);
    // flip
    cv::flip(mask, mask, -1);

    /// remove small fragments
    AgribotVS::filterSmallFragments(mask, minSegmentSize);

    AgribotVS::calcRGBVI(red, green, blue, rgbVi, false);
  }

  void AgribotVS::calcRGBVI(const cv::Mat& red, const cv::Mat& green,
                                    const cv::Mat& blue, cv::Mat& rgbVi,
                                    const bool& cutDistribution) {
    if (!cutDistribution) {
      //std::cerr << "AgribotVS::calcRGBVI:: Exg conventional"
      //          << std::endl;

      cv::Mat redConv, greenConv, blueConv;
      red.convertTo(redConv, CV_32FC1);
      green.convertTo(greenConv, CV_32FC1);
      blue.convertTo(blueConv, CV_32FC1);

      cv::Mat nR = redConv / 255.f;
      cv::Mat nG = greenConv / 255.f;
      ;
      cv::Mat nB = blueConv / 255.f;
      ;
      cv::Mat norm = nR + nG + nB;

      cv::Mat cR = nR / norm;
      cv::Mat cG = nG / norm;
      cv::Mat cB = nB / norm;

      /**
      * get the excessive green index [ExG]
      */
      rgbVi = 2 * cG - cR - cB;
      rgbVi.convertTo(rgbVi, CV_8UC1, 127.5, 127.5);
      cv::normalize(rgbVi, rgbVi, 0, 255, cv::NORM_MINMAX);

    } else {
      //std::cerr << "AgribotVS::calcRGBVI:: Exg magic cut" << std::endl;

      /**
      * get the excessive green index [ExG]
      */
      rgbVi = 2 * green - red - blue;
    }

    cv::medianBlur(rgbVi, rgbVi, 3);
  }

  void AgribotVS::preProcessImageData(cv::Mat& rgb) {
    cv::Mat red, green, blue;
    AgribotVS::splitRgbImage(rgb, red, green, blue);

    AgribotVS::preProcessRGBdata(red, green, blue);

    AgribotVS::mergeRgbImage(rgb, red, green, blue);
  }

  void AgribotVS::filterSmallFragments(cv::Mat& noisyImage,
                                               const int minFragmentSize) {
    // init container
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    // find contours
    cv::findContours(noisyImage.clone(), contours, hierarchy, CV_RETR_EXTERNAL,
                    CV_CHAIN_APPROX_NONE);

    for (int i = 0; i < contours.size(); ++i) {
      if (cv::contourArea(contours[i]) <= minFragmentSize) {
        cv::drawContours(noisyImage, contours, i, 0, -1);
      }
    }
  }

  //*************************************************************************************************

  vector<Vec4i> AgribotVS::EdgeDetector(Mat& Input_Image, float Scale) {
    Mat src;
    Mat gray, cdst, blur;
    vector<Vec4i> lines;
    vector<Vec4i> path;
    cv::resize(Input_Image, src, cv::Size(), Scale, Scale);

    //cout << "Cols: " << src.cols << ", Rows: " << src.rows << endl;
    if (src.channels() == 3) {
      cvtColor(src, gray, CV_BGR2GRAY);
    } else {
      gray = src;
    }
    // GaussianBlur(gray, cdst , Size( 2, 2 ), 0,0);
    // Canny(blur, cdst, 50, 0, 3);
    adaptiveThreshold(~gray, cdst, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,
                      15, -2);
    // Show binary image
    // imshow("binary", cdst);
    // Create the images that will use to extract the horizontal and vertical
    // lines
    Mat vertical = cdst.clone();
    // Specify size on vertical axis
    int verticalsize = vertical.rows / 10;
    // Create structure element for extracting vertical lines through
    // morphology
    // operations
    Mat verticalStructure =
        getStructuringElement(MORPH_RECT, Size(1, verticalsize));
    // Apply morphology operations
    erode(vertical, vertical, verticalStructure, Point(-1, -1));
    dilate(vertical, vertical, verticalStructure, Point(-1, -1));
    // Show extracted vertical lines
    // imshow("vertical", vertical);
    path.resize(1, 0);
    path[0][0] = CenterLine[0][0];
    path[0][1] = CenterLine[0][1];
    path[0][2] = CenterLine[0][2];
    path[0][3] = CenterLine[0][3];
    HoughLinesP(vertical, lines, 1, CV_PI / 360, 10, 0,
                0);  //(vertical, lines, 0.5, CV_PI / 360, 100, 100, 150);
    if (lines.size() > 1) {
      for (size_t i = 1; i < lines.size(); i++) {
        line(src, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(255, 0, 255), 2, CV_AA);
      }
      path[0][0] = (lines[0][0] + lines[1][0]) / 2;
      path[0][1] = (lines[0][1] + lines[1][1]) / 2;
      path[0][2] = (lines[0][2] + lines[1][2]) / 2;
      path[0][3] = (lines[0][3] + lines[1][3]) / 2;
      line(src, Point(path[0][0], path[0][1]), Point(path[0][2], path[0][3]),
          Scalar(0, 255, 255), 4, CV_AA);
      line(src, Point(CenterLine[0][0], CenterLine[0][1]),
          Point(CenterLine[0][2], CenterLine[0][3]), Scalar(0, 255, 0), 4,
          CV_AA);
    }

    Input_Image = src.clone();
    return lines;
  }

  Mat AgribotVS::CropImage(Mat source, uint x_ff, uint y_ff, uint width, uint height) {
    // cout << "souce --- Cols: " << source.cols << ", Rows: " << source.rows <<
    // endl;
    // Setup a rectangle to define your region of interest
    cv::Rect myROI(x_ff, y_ff, width, height);

    // Crop the full image to that image contained by the rectangle myROI
    cv::Mat croppedRef(source, myROI);

    Mat cropped;
    // Copy the data into new matrix
    croppedRef.copyTo(cropped);
    // cout << "Cropped ::::  Cols: " << cropped.cols << ", Rows: " <<
    // cropped.rows << endl;
    return cropped;
  }

  void AgribotVS::CicularTrajectroy(double radius, int steps){
    float x_offset = RobotPose[0];
    float y_offset = RobotPose[1];
    for (int theta = 0; theta < M_PI; M_PI/steps++){
      float _x = x_offset + radius * cos(theta);
      float _y = y_offset + radius * sin(theta);
      SetNewGoal(_x, _y, theta);
    }
  }

  float AgribotVS::AngleDifference(Mat& Input_Image, vector<Vec4i> lines) {
    string str;
    float angle = 0.0f;
    Vec4i l = lines[0];
    Point p1, p2;
    p1 = Point(l[0], l[1]);
    p2 = Point(l[2], l[3]);
    try {
      angle = atan2(p1.y - p2.y, p1.x - p2.x);
      if(angle > 0){
        angle -= M_PI/2;
      }else if(angle < 0){
        angle += M_PI/2;
      }
      stringstream stream;
      stream << angle;
      str = "Angle: " + stream.str();
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
    }
    cv::putText(Input_Image, str, cv::Point(20, 20),  // Coordinates
                cv::FONT_HERSHEY_COMPLEX_SMALL,       // Font
                0.75,                                 // Scale. 2.0 = 2x bigger
                cv::Scalar(0, 0, 255),                // BGR Color
                1);                                   // Line Thickness (Optional)
    return angle;
  }

  void AgribotVS::ComputeVelocities(Mat& Input_Image, float lineAngle, vector<Vec4i> lines) {
    string str;
    //cout << lines[0][0] << "  " <<lines[0][1]  << "  " << lines[0][2] << "  " <<lines[0][3] << endl;
    if(x > Input_Image.cols/2 + center_max_off/6){
      VelocityMsg.linear.x = LinearPIDController(lineAngle, RobotPose, RobotAngularVelocities);
      VelocityMsg.angular.z = -0.07;
    }else if(x < Input_Image.cols/2 - center_min_off/6){
      VelocityMsg.linear.x = LinearPIDController(lineAngle, RobotPose, RobotAngularVelocities);
      VelocityMsg.angular.z = 0.07;
    }else{
      if(abs(lineAngle) < LineDiffOffset){
        VelocityMsg.linear.x = LinearPIDController(lineAngle, RobotPose, RobotAngularVelocities);
        VelocityMsg.angular.z = 0.0;
      }else{
          VelocityMsg.linear.x = LinearPIDController(lineAngle, RobotPose, RobotAngularVelocities);
          VelocityMsg.angular.z = AngularPIDController(lineAngle, RobotPose, RobotAngularVelocities);
      }
    }

    // put texts on the screen
    stringstream xstream, zstream;
    xstream << VelocityMsg.linear.x;
    zstream << VelocityMsg.angular.z;
    str = "x: " + xstream.str() + ", z: " + zstream.str();
    cv::putText(Input_Image, str, cv::Point(20, 35),  // Coordinates
                cv::FONT_HERSHEY_COMPLEX_SMALL,       // Font
                0.75,                                 // Scale. 2.0 = 2x bigger
                cv::Scalar(255, 0, 255),              // BGR Color
                1);                                   // Line Thickness (Optional)
  }

  double AgribotVS::LinearPIDController(double LineAngle, vector<double> RobotPose_t,vector<double> RobotLinearVelocities_t) {
    double vel_curr =
        hypot(RobotLinearVelocities_t[1], RobotLinearVelocities_t[0]);
    // cout << vel_curr << endl;
    double vel_target =
        hypot(RobotPose_t[0] + 0.3, RobotPose_t[1] + 0.3) /
        d_t_;  // linear velocity of depends on the p_windows (direct relation)

    if (fabs(vel_target) > max_vel_lin_) {
      vel_target = copysign(max_vel_lin_, vel_target);
    }

    double err_vel = vel_target - vel_curr;

    integral_lin_ += err_vel * d_t_;
    double derivative_lin = (err_vel - error_lin_) / d_t_;
    double incr_lin =
        k_p_lin_ * err_vel + k_i_lin_ * integral_lin_ + k_d_lin_ * derivative_lin;
    error_lin_ = err_vel;

    if (fabs(incr_lin) > max_incr_lin_) incr_lin = copysign(max_incr_lin_, incr_lin);

    double x_velocity = vel_curr + incr_lin;
    if(abs(x_velocity) > max_vel_lin_) x_velocity = copysign(max_vel_lin_, x_velocity);
    if(abs(x_velocity) < min_vel_lin_) x_velocity = copysign(min_vel_lin_, x_velocity);

    return x_velocity;
  }

  double AgribotVS::AngularPIDController(double LineAngle, vector<double> RobotPose_t,vector<double> RobotAngularVelocities_t) {
    double target_vel_ang = (LineAngle - RobotPose_t[2]) / d_t_;
    if (fabs(target_vel_ang) > max_vel_ang_) {
      target_vel_ang = copysign(max_vel_ang_, target_vel_ang);
    }
    double vel_ang = RobotAngularVelocities_t[2];
    double error_ang = target_vel_ang - vel_ang;
    integral_ang_ += error_ang * d_t_;
    double derivative_ang = (error_ang - error_ang_) / d_t_;
    double incr_ang = k_p_ang_ * error_ang + k_i_ang_ * integral_ang_ +
                      k_d_ang_ * derivative_ang;
    error_ang_ = error_ang;

    if (fabs(incr_ang) > max_incr_ang_) incr_ang = copysign(max_incr_ang_, incr_ang);

    double th_velocity = copysign(vel_ang + incr_ang, target_vel_ang);
    if (fabs(th_velocity) > max_vel_ang_) th_velocity = copysign(max_vel_ang_, th_velocity);
    if (fabs(th_velocity) < min_vel_ang_) th_velocity = copysign(min_vel_ang_, th_velocity);
    return th_velocity;
  }

  std::vector<double> AgribotVS::getEulerAngles(const nav_msgs::Odometry::ConstPtr& Pose) {
    std::vector<double> EulerAngles;
    EulerAngles.resize(3, 0);
    tf::Quaternion q(Pose->pose.pose.orientation.x, Pose->pose.pose.orientation.y,
                    Pose->pose.pose.orientation.z,
                    Pose->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    m.getRPY(EulerAngles[0], EulerAngles[1], EulerAngles[2]);
    return EulerAngles;
  }

  bool AgribotVS::SetNewGoal(const float& x, const float& y, const float& angle) {
    ROS_INFO("New goal is sending ....................");
    // Initialize Move base client
    MoveBaseClient ac("move_base", true);
    // waiting for server to start
    while (!ac.waitForServer(ros::Duration(20))) {
      ROS_INFO("waiting for the move_base action server");
    }
    // Declaring move base goal
    move_base_msgs::MoveBaseGoal goal;

    // Setting targer frame id and time in the goal action
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    // Retrieving pose for command line other vice execute a defualt value

    goal.target_pose.pose.position.x = 5+ x;
    goal.target_pose.pose.position.y = 5+ y;

    tf::Quaternion quaternion;
    quaternion = tf::createQuaternionFromYaw(angle);

    geometry_msgs::Quaternion qMsg;
    tf::quaternionTFToMsg(quaternion, qMsg);

    goal.target_pose.pose.orientation = qMsg;

    // ROS_INFO("Current pose is: x = %f, y = %f, theta = %f",
    //          5 + tVec.at<double>(0), 5 + tVec.at<double>(1), tVec.at<double>(2));

    ROS_INFO("Sending goal to: x = %f, y = %f, theta = %f", goal.target_pose.pose.position.x, goal.target_pose.pose.position.y, angle);

    // Sending goal
    ac.sendGoal(goal);
    // double pose_offset = 0.1;
    // if (TransVec[0] <= x_poses[0] + pose_offset &&
    //     TransVec[0] >= x_poses[0] - pose_offset) {
    //     if (TransVec[1] <= y_poses[0] + pose_offset &&
    //         TransVec[1] >= y_poses[0] - pose_offset) {
    //   ROS_INFO("thershold -> Robot has arrived to the goal postion");
    //   return true;
    //     }
    // }
    ac.waitForResult();

    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
      ROS_INFO("Robot has arrived to the goal postion");
      return true;
    } else {
      ROS_INFO("The base failed for some reason...");
      return false;
    }
  }

}  // namespace agribot_vs
