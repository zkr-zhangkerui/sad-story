clc;
clear all;

%% 绘制传感器数据
currentData = readmatrix("sensors_matrix.txt", ...
    'FileType', 'text', 'NumHeaderLines', 0);
currentData = double(currentData);

time=currentData(:,1);
data_g=currentData(:,2:4);%陀螺 deg/s
data_a=currentData(:,5:7);%加速度计 m/ss
data_m=currentData(:,8:10)/1000;%磁强计 Gauss

figure('Units', 'centimeters', 'Position', [1, 1, 16*1.3, 12*1.3]);
subplot(3,1,1);
plot(time,data_g);
legend("GX","GY","GZ",'FontSize',9*1.3);
ylabel("deg/s",'FontSize',9*1.3);
title("(a)陀螺数据",'FontSize',9.5*1.3);

subplot(3,1,2);
plot(time,data_a);
legend("AX","AY","AZ",'FontSize',9*1.3);
ylabel("m/s^2",'FontSize',9*1.3);
title("(b)加速度计数据",'FontSize',9.5*1.3);

subplot(3,1,3);
plot(time,data_m);
legend("MX","MY","MZ",'FontSize',9*1.3);
xlabel("时间(s)",'FontSize',9*1.3);
ylabel("Gauss",'FontSize',9*1.3);
title("(c)磁强计数据",'FontSize',9.5*1.3);

%% 绘制真实数据
true_pos=load("pos_reference.txt");
true_pos(:,4)=0;
for i=1:height(true_pos)
    true_pos_xyz(i,:)=llh2xyz(true_pos(i,2:4));
end
for i=1:height(true_pos)
    true_pos_enu(i,:)=xyz2enu(true_pos_xyz(i,:),true_pos_xyz(1,:));
end
%% 使用加速度计数据计算水平姿态角
for i=1:height(data_a)
    angle(i,1)=atan2(-data_a(i,2),-data_a(i,3));%r
    angle(i,2)=atan2(data_a(i,1),sqrt(data_a(i,3)^2+data_a(i,2)^2));%θ
end

%1s滑动平均
angle_sec = movmean(angle, [20-1, 0]); 

figure;
subplot(2,1,1);
plot(1:height(angle),angle(:,1)*180/pi,1:height(angle),angle(:,2)*180/pi);
xlabel("历元");
ylabel("每历元姿态角(°)");
legend("横滚角","航向角");
subplot(2,1,2);
plot(1:height(angle_sec),angle_sec(:,1)*180/pi,1:height(angle_sec),angle_sec(:,2)*180/pi);
xlabel("历元(s)");
ylabel("1s滑动平均姿态角(°)");
legend("横滚角","航向角");

%% 磁强计和陀螺数据计算航向角
for i=1:height(angle_sec)
    C=[cos(angle_sec(i,2)),sin(angle_sec(i,1))*sin(angle_sec(i,2)),cos(angle_sec(i,1))*sin(angle_sec(i,2));
    0,cos(angle_sec(i,1)),-sin(angle_sec(i,1));
    -sin(angle_sec(i,2)),sin(angle_sec(i,1))*cos(angle_sec(i,2)),cos(angle_sec(i,1))*cos(angle_sec(i,2))];
    data_m_n1(i,1)=C(1,:)*data_m(i,:)';
    data_m_n1(i,2)=C(2,:)*data_m(i,:)';
    yaw(i,1)=-atan2(data_m_n1(i,2),data_m_n1(i,1))+14.11*pi/180;
    if(yaw(i,1)>=pi)
        yaw(i,1)=yaw(i,1)-2*pi;
    end
    if(yaw(i,1)<-pi)
        yaw(i,1)=yaw(i,1)+2*pi;
    end
end

yaw_g(1,1)=-pi/2;
for i=2:height(angle_sec)
    C=[cos(angle_sec(i-1,2)),sin(angle_sec(i-1,1))*sin(angle_sec(i-1,2)),cos(angle_sec(i-1,1))*sin(angle_sec(i-1,2));
    0,cos(angle_sec(i-1,1)),-sin(angle_sec(i-1,1));
    -sin(angle_sec(i-1,2)),sin(angle_sec(i-1,1))*cos(angle_sec(i-1,2)),cos(angle_sec(i-1,1))*cos(angle_sec(i-1,2))];
    data_g_n(i,:)=C(3,:)*data_g(i-1,:)';
    yaw_g(i,1)=yaw_g(i-1,1)+data_g_n(i,1)*pi*0.05/180;
    if(yaw_g(i,1)>=pi)
        yaw_g(i,1)=yaw_g(i,1)-2*pi;
    end
    if(yaw_g(i,1)<-pi)
        yaw_g(i,1)=yaw_g(i,1)+2*pi;
    end
end

figure;
plot(1:height(yaw),yaw*180/pi,1:height(yaw_g),yaw_g*180/pi);
xlabel("历元");
ylabel("航向角(°)");
legend("磁强计","陀螺");

%% 脚步探测
for i=1:height(data_a)
    norm_a(i,1)=sqrt(data_a(i,1)^2+data_a(i,2)^2+data_a(i,3)^2);
end
% 使用findpeaks参数添加约束
min_peak_prominence = 0.4;  % 最小峰值突出度，根据数据调整
min_peak_distance = 8;     % 最小峰值距离，根据步频
[peaks, peak_locs, ~, prominences] = findpeaks(-norm_a, ...
    'MinPeakProminence', min_peak_prominence, ...
    'MinPeakDistance', min_peak_distance);

troughs = -peaks;  % 转换回实际值
trough_index= peak_locs;
step=zeros([height(data_a),1]);
for i=1:height(trough_index)
    if troughs(i,1)<9.2
        step(trough_index(i),1)=troughs(i,1);
    else
        step(trough_index(i),1)=NaN;
    end
end
step(step==0)=NaN;

figure;
hold on;
plot(1:height(norm_a),norm_a(:,1));
scatter(1:height(step),step(:,1),5,'filled');
xlabel("历元");
ylabel("加速度计数据模长(m/s^2)");
legend("加速度计模长","脚步");

%% 航迹推算 
pos(i,1)=0;pos(i,2)=0;time0=1;pos_m(i,1)=0;pos_m(i,2)=0;
for i=2:height(yaw_g)
    if ~isnan(step(i-1,1))
        %陀螺
        pos(i,1)=pos(time0,1)+0.7*sin(yaw_g(time0));%E
        pos(i,2)=pos(time0,2)+0.7*cos(yaw_g(time0));%N
        %磁强计
        pos_m(i,1)=pos_m(time0,1)+0.7*sin(yaw(time0));%E
        pos_m(i,2)=pos_m(time0,2)+0.7*cos(yaw(time0));%N
        time0=i;
    end
end
pos(pos==0)=NaN;
pos_m(pos_m==0)=NaN;

figure;
hold on;
scatter(pos(:,1),pos(:,2),3,'filled');
scatter(pos_m(:,1),pos_m(:,2),3,'filled');
scatter(true_pos_enu(:,1),true_pos_enu(:,2),3,'filled');
xlabel("东向(m)");
ylabel("北向(m)");
legend("陀螺","磁强计","真实轨迹");
axis equal; 

%% 通过标定陀螺零偏对陀螺数据进行误差补偿
% 通过与磁强计的航向差异估计零偏
% 定义零偏搜索范围
bias_candidates = linspace(-0.02, 0.02, 50);  % rad/s
rmse_errors = zeros(size(bias_candidates));

for j = 1:length(bias_candidates)
    % 应用当前零偏补偿
    gyro_comp = data_g;
    gyro_comp(:,3) = data_g(:,3) - bias_candidates(j)*180/pi;
    %计算陀螺航向
    yaw_g_bias(1,1)=-pi/2;
    for i=2:height(angle_sec)
        C=[cos(angle_sec(i-1,2)),sin(angle_sec(i-1,1))*sin(angle_sec(i-1,2)),cos(angle_sec(i-1,1))*sin(angle_sec(i-1,2));
        0,cos(angle_sec(i-1,1)),-sin(angle_sec(i-1,1));
        -sin(angle_sec(i-1,2)),sin(angle_sec(i-1,1))*cos(angle_sec(i-1,2)),cos(angle_sec(i-1,1))*cos(angle_sec(i-1,2))];
        data_g_n(i,:)=C(3,:)*gyro_comp(i-1,:)';
        yaw_g_bias(i,1)=yaw_g_bias(i-1,1)+data_g_n(i,1)*pi*0.05/180;
        if(yaw_g_bias(i,1)>=pi)
            yaw_g_bias(i,1)=yaw_g_bias(i,1)-2*pi;
        end
        if(yaw_g_bias(i,1)<-pi)
            yaw_g_bias(i,1)=yaw_g_bias(i,1)+2*pi;
        end
    end
    % 计算与磁力计航向的RMSE
    valid_idx = ~isnan(yaw) & ~isnan(yaw_g_bias);
    if sum(valid_idx) > 100
        rmse_errors(j) = sqrt(mean((yaw_g_bias(valid_idx) - yaw(valid_idx)).^2));
    else
        rmse_errors(j) = inf;
    end
end
% 找到最小误差对应的零偏
[min_error, min_idx] = min(rmse_errors);
bias_z = bias_candidates(min_idx);

data_g_bias(:,3)=data_g(:,3)-bias_z*180/pi;

%% 补偿后陀螺数据计算航向角
yaw_g_bias(1,1)=-pi/2;
for i=2:height(angle_sec)
    C=[cos(angle_sec(i-1,2)),sin(angle_sec(i-1,1))*sin(angle_sec(i-1,2)),cos(angle_sec(i-1,1))*sin(angle_sec(i-1,2));
    0,cos(angle_sec(i-1,1)),-sin(angle_sec(i-1,1));
    -sin(angle_sec(i-1,2)),sin(angle_sec(i-1,1))*cos(angle_sec(i-1,2)),cos(angle_sec(i-1,1))*cos(angle_sec(i-1,2))];
    data_g_n(i,:)=C(3,:)*data_g_bias(i-1,:)';
    yaw_g_bias(i,1)=yaw_g_bias(i-1,1)+data_g_n(i,1)*pi*0.05/180;
    if(yaw_g_bias(i,1)>=pi)
        yaw_g_bias(i,1)=yaw_g_bias(i,1)-2*pi;
    end
    if(yaw_g_bias(i,1)<-pi)
        yaw_g_bias(i,1)=yaw_g_bias(i,1)+2*pi;
    end
end

figure;
plot(1:height(yaw),yaw*180/pi,1:height(yaw_g_bias),yaw_g_bias*180/pi);
xlabel("历元");
ylabel("航向角(°)");
legend("磁强计","陀螺");

%% 陀螺补偿后航迹推算 
pos_bias(i,1)=0;pos_bias(i,2)=0;time0=1;
for i=2:height(yaw_g_bias)
    if ~isnan(step(i-1,1))
        %陀螺
        pos_bias(i,1)=pos_bias(time0,1)+0.7*sin(yaw_g_bias(time0));%E
        pos_bias(i,2)=pos_bias(time0,2)+0.7*cos(yaw_g_bias(time0));%N
        time0=i;
    end
end
pos_bias(pos_bias==0)=NaN;

figure;
hold on;
scatter(pos_bias(:,1),pos_bias(:,2),3,'filled');
scatter(pos_m(:,1),pos_m(:,2),3,'filled');
scatter(true_pos_enu(:,1),true_pos_enu(:,2),3,'filled');
xlabel("东向(m)");
ylabel("北向(m)");
legend("陀螺","磁强计","真实轨迹");
axis equal; 