#include<ros/ros.h>
#include<geometry_msgs/Point.h>//包含Point消息类型，用于表示三维空间中的点
#include<actionlib/client/simple_action_client.h>
#include<turtlebot3_nav/TurtleBot3NavigateToGoalAction.h>
#include<turtlebot3_nav/SetGoal.h>

//定义了一个别名
typedef
actionlib::SimpleActionClient<turtlebot3_nav::TurtleBot3NavigateToGoalAction> NavigateActionClient;
// 定义全局变量
//NavigateActionClient ac("navigate_to_goal", true);
//反馈回调函数
void feedbackCallback(const turtlebot3_nav::TurtleBot3NavigateToGoalFeedbackConstPtr& feedback) 
{
    ROS_INFO("Current Pose: (%.2f, %.2f)", feedback->current_pose.position.x, feedback->current_pose.position.y);
    ROS_INFO("Distance to Goal: %.2f", feedback->distance_to_goal);
}
//定义服务回调函数
bool setGoal(turtlebot3_nav::SetGoal::Request &req,turtlebot3_nav::SetGoal::Response &res)
{
    //创建动作客户端
    NavigateActionClient ac("navigate_to_goal",true);
    ROS_INFO("waiting for action server to start");

    if(!ac.waitForServer(ros::Duration(30.0)))
    {
        ROS_ERROR("action server not available within 30s");
        res.success=false;
        res.message="Action server not available";
        return true;
    }

    //创建一个动作目标对象,并设置目标位姿
    turtlebot3_nav::TurtleBot3NavigateToGoalGoal goal;
    goal.target_pos.header.frame_id="map";//坐标系为map
    goal.target_pos.header.stamp=ros::Time::now();//时间戳为当前时间
    goal.target_pos.pose.position =req.goal;//服务请求中的目标点设置为目标位姿的位置
    goal.target_pos.pose.orientation.x = 0;
    goal.target_pos.pose.orientation.y = 0;
    goal.target_pos.pose.orientation.z = 0;
    goal.target_pos.pose.orientation.w = 1.0; // 确保四元数有效

    ac.sendGoal(goal,NULL, NULL,&feedbackCallback);
    if (!ac.waitForResult(ros::Duration(60.0))) 
    {
        ROS_ERROR("Action did not finish before the timeout.");
        res.success = false;
        res.message = "Timeout";
        return true;
    }
    ROS_INFO("action server started");
    //res.message="action server strarted";

    //获取动作服务器的状态
    if(ac.getState()==actionlib::SimpleClientGoalState::SUCCEEDED)
    {
        res.success=true;
        res.message="Navigation completed";
        ROS_INFO("Navigation completed successfully: %s", res.message.c_str());
        return true;
    }
    else
    {
        res.success=false;
        res.message="Navigation failed";
    }

    return true;
}
int main(int argc,char **argv)
{
    ros::init(argc,argv,"goal_service_server");
    ros::NodeHandle nh;
    
    //创建一个服务服务器，监听名为set_goal的服务请求，并将回调函数设置为setGoal
    ros::ServiceServer service=nh.advertiseService("set_goal",setGoal);
    ROS_INFO("set goal service is ready");
    ros::spin();
    return 0;
}
