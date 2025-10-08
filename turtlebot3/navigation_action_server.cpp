#include<ros/ros.h>
#include<actionlib/server/simple_action_server.h>
#include<turtlebot3_nav/TurtleBot3NavigateToGoalAction.h>
#include<geometry_msgs/PoseStamped.h>//用于表示带有时间戳和坐标系的位姿信息
#include<geometry_msgs/Twist.h>
#include<tf/tf.h>
#include<tf/transform_listener.h>
#include<tf/transform_datatypes.h>//用于转换
#include<thread>

//定义一个别名
typedef
actionlib::SimpleActionServer<turtlebot3_nav::TurtleBot3NavigateToGoalAction> NavigateActionServer;

class NavigationActionServer
{
public:
    //创建一个动作服务器
    NavigationActionServer(ros::NodeHandle nh):as_(nh,"navigate_to_goal",false),nh_(nh),is_navigating_(false)
    {
        //注册目标回调函数，当客户端发送目标时，调用goalCallback函数
        as_.registerGoalCallback(boost::bind(&NavigationActionServer::goalCallback,this));
        //注册取消回调函数，当客户端取消任务时，调用preemptCallback函数
        as_.registerPreemptCallback(boost::bind(&NavigationActionServer::preemptCallback,this));
        as_.start();
    }

    void spin()
    {
        //ROS_INFO("[ActionServer] Entering control loop...");
        ros::Rate rate(10);
        while(ros::ok()&&is_navigating_)
        
        {
            try
            {
                // 创建一个StampedTransform变量来接收lookupTransform的结果
                tf::StampedTransform transform;
                //查询map坐标系到base_footprint坐标系的变换，并将其保存到current_pose_中
                tf_listener_.lookupTransform("map","base_footprint",ros::Time(0),transform);
                //将StampedTransform转换为PoseStamped
                current_pose_.header.frame_id="map";
                current_pose_.header.stamp=ros::Time::now();
                current_pose_.pose.position.x=transform.getOrigin().x();
                current_pose_.pose.position.y=transform.getOrigin().y();
                current_pose_.pose.position.z=transform.getOrigin().z();
                //获取旋转部分
                tf::Quaternion q;
                transform.getBasis().getRotation(q);
                q.normalize();  // 规范化四元数
                current_pose_.pose.orientation.x=q.x();
                current_pose_.pose.orientation.y=q.y();
                current_pose_.pose.orientation.z=q.z();
                current_pose_.pose.orientation.w=q.w();

            }
            //捕获可能发生的坐标变换异常
            catch(tf::TransformException &ex)
            {
                ROS_ERROR("%s",ex.what());
                rate.sleep();
                continue;
            }
            //计算机器人当前位置到目标位置的距离
            float distance_to_goal=sqrt(pow(target_pose_.pose.position.x-current_pose_.pose.position.x,2)+pow(target_pose_.pose.position.y-current_pose_.pose.position.y,2));
            //ROS_INFO("target:(%.2f,%.2f)",target_pose_.pose.position.x,target_pose_.pose.position.y);
            ROS_INFO("Distance to goal: %.2f", distance_to_goal);
            if(distance_to_goal<0.1)
            {
                /*
                is_navigating_=false;
                as_.setSucceeded();
                break;*/
                is_navigating_ = false;
                
                geometry_msgs::Twist cmd_vel_stop;
                cmd_vel_stop.linear.x = 0.0;
                cmd_vel_stop.angular.z = 0.0;
                cmd_vel_pub_.publish(cmd_vel_stop);
                
                ros::spinOnce(); // 确保停止指令发送
                
                as_.setSucceeded();
                break;
            }
            //计算速度
            geometry_msgs::Twist cmd_vel;
            float angle_error=atan2(target_pose_.pose.position.y-current_pose_.pose.position.y,target_pose_.pose.position.x-current_pose_.pose.position.x)-tf::getYaw(current_pose_.pose.orientation);
            if(angle_error>M_PI)
            {
                angle_error-=2*M_PI;
            }
            if(angle_error<-M_PI)
            {
                angle_error+=2*M_PI;
            }
            
            cmd_vel.angular.z=0.5*angle_error;
            //当目标正确时前进
            if(fabs(angle_error)<0.1)
            {
                cmd_vel.linear.x=distance_to_goal;
            }
            else
            {
                cmd_vel.linear.x=0;
            }
            cmd_vel_pub_.publish(cmd_vel);
            //反馈消息
            turtlebot3_nav::TurtleBot3NavigateToGoalFeedback feedback;
            feedback.current_pose.position = current_pose_.pose.position;
            feedback.current_pose.orientation=current_pose_.pose.orientation;
            feedback.distance_to_goal=distance_to_goal;
            as_.publishFeedback(feedback);

            rate.sleep();
        }
    }
private:
    NavigateActionServer as_;//动作服务器对象
    ros::NodeHandle nh_;
    ros::Publisher cmd_vel_pub_;//用于发布速度命令的发布器
    tf::TransformListener tf_listener_;//用于监听坐标变换的监听器
    geometry_msgs::PoseStamped current_pose_;
    geometry_msgs::PoseStamped target_pose_;
    bool is_navigating_;

    //客户端发送目标时,调用此函数
    void goalCallback()
    {
        is_navigating_=true;//表示开始导航
        try
        {
            target_pose_ = as_.acceptNewGoal()->target_pos;//获取客户端发送的目标位姿
            cmd_vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel",1);//发布速度命令到cmd_vel话题
        
            ROS_INFO("New goal received: (%.2f, %.2f)", target_pose_.pose.position.x, target_pose_.pose.position.y);
        }
        catch(const std::exception& e)
        {
            ROS_ERROR("[ActionServer] Failed to accept goal: %s", e.what());
        }
        
        
    }

    //客户端取消任务时,调用此函数
    void preemptCallback()
    {
        is_navigating_=false;
        as_.setPreempted();
    }

};
int main(int argc,char **argv)
{
    //初始化ROS节点，命名为navigation_action_server
    ros::init(argc,argv,"navigation_action_server");
    ros::NodeHandle nh;
    //创建一个导航动作服务器对象
    NavigationActionServer server(nh);
    ros::Rate rate(10);
    while (ros::ok()) 
    {
        ros::spinOnce(); // 处理所有待处理的ROS消息
        server.spin();   // 运行导航服务器的主循环
        rate.sleep();
    }
    return 0;
}
