
function [Q,pos] = trilaterate(XYZ, ranges,model)
    % 初始猜测：用四个站的平均位置
    pos0 = mean(XYZ, 1);
    
    % 计算几何距离
    dist = vecnorm(XYZ - pos0, 2, 2);

    % 构造设计矩阵 H 和残差向量 v
    H = -(XYZ - pos0)./dist;  % 4×2
    if model==1
        v = ranges(1:4)' - dist;        % 4×1
    else
        v = ranges' - dist; 
    end

    if model==2%增加零偏
        H(:,width(H)+1)=1;
        pos0(width(pos0)+1)=0;
        XYZ(:,width(XYZ)+1)=0;
    end

    % 迭代最小二乘
    for iter = 1:10
        % 计算几何距离
        dist = vecnorm(XYZ - pos0, 2, 2);
        
        % 构造设计矩阵 H 和残差向量 v
        H = -(XYZ - pos0)./dist;  % 4×2
        if model==1
            v = ranges(1:4)' - dist;        % 4×1
        else
            v = ranges' - dist;
        end
        if model==1
            H(size(H,1)+1,3)=1;
            v(size(v,1)+1)=ranges(5);
        end

        % 最小二乘更新
        dx = (H' * H) \ (H' * v);
        pos0 = pos0 + dx';
        
        if norm(dx) < 1e-4
            break;
        end
    end
    Q=inv(H' * H);
    pos = pos0;
end

%% 超声波
%读取文件
clc;clear;
filePath_base="数据1-无人机-Ultrasonic\base-station-location.txt";
filePath_rove="数据1-无人机-Ultrasonic\range.txt";
filePath_height="数据1-无人机-Ultrasonic\range-with-height.txt";

data_base=load(filePath_base);
data_rove=load(filePath_rove);
data_height=load(filePath_height);

stations=data_base(:,2:3);
dis=data_rove(:,2:5);
dis_height=data_height(:,2:5);%水平距离
height=data_height(:,6);
time=data_rove(:,1);

stations_xyz=zeros(4,3);%stations_enu=zeros(4,3);
for i=1:4
    stations_xyz(i,:)=llh2xyz(data_base(i,2:4));
    stations_enu(i,:)=xyz2enu(stations_xyz(i,:),stations_xyz(1,:));
end
for i=1:size(dis_height,1)
    for j=1:width(dis_height)
        dis_height(i,j)=sqrt(dis_height(i,j)^2-height(i,1)^2);
    end
end

%最小二乘解算
%二维定位 在距离中减去已知高程，只进行水平方向定位
for i=1:size(data_rove,1)
    [Q1(i,:,:),result1(i,:)]=trilaterate(stations_enu(:,1:2),dis_height(i,:),0);
end
%直接三维定位
for i=1:size(data_rove,1)
    %[Q2(i,:,:),result2(i,:)]=trilaterate(stations_enu(:,1:3),dis(i,:),0);
end
%三位定位+高程约束
for i=1:size(data_rove,1)
    [Q3(i,:,:),result3(i,:)]=trilaterate(stations_enu(:,1:3),data_height(i,2:6),1);
end
%二维定位+零偏
for i=1:size(data_rove,1)
    %[Q4(i,:,:),result4(i,:)]=trilaterate(stations_enu(:,1:2),dis_height(i,:),2);
end

%二维定位
figure;
%scatter(result_height(:,1), result_height(:,2),4,'filled');
hold on;
scatter(result1(:,1), result1(:,2),4,'filled');
scatter(stations_enu(:,1),stations_enu(:,2),100,'*');
scatter(result3(:,1), result3(:,2),4,'filled');
legend("二维定位","测站","三维定位+高程约束");
xlabel('E(m)');
ylabel('N(m)');
ylim([-16,8]);
grid on;
axis equal;            

%所有历元平均水平方向误差椭圆
Q1=mean(Q1,1);
Q3=mean(Q3,1);
a1=sqrt(0.5*(Q1(1,1,1)+Q1(1,2,2))+sqrt(0.25*((Q1(1,1,1)-Q1(1,2,2))^2)+Q1(1,2,1)));
a2=sqrt(0.5*(Q1(1,1,1)+Q1(1,2,2))-sqrt(0.25*((Q1(1,1,1)-Q1(1,2,2))^2)+Q1(1,2,1)));
a1_height=sqrt(0.5*(Q3(1,1,1)+Q3(1,2,2))+sqrt(0.25*((Q3(1,1,1)-Q3(1,2,2))^2)+Q3(1,2,1)));
a2_height=sqrt(0.5*(Q3(1,1,1)+Q3(1,2,2))-sqrt(0.25*((Q3(1,1,1)-Q3(1,2,2))^2)+Q3(1,2,1)));
% 参数方程
theta = linspace(0, 2*pi, 100);
x = 0 + a1 * cos(theta);
y = 0 + a2 * sin(theta);
x_height = 0 + a1_height * cos(theta);
y_height = 0 + a2_height * sin(theta);

figure;
plot(x, y,x_height,y_height,'LineWidth', 2);
axis equal;  % 重要：保持纵横比相等
grid on;
xlabel('X(m)');
ylabel('Y(m)');
title('水平方向误差椭圆');
legend("二维定位","三维定位+高程约束");
axis equal;

figure;
subplot(2,1,1);
hold on;
for i=1:width(dis)
    plot(1:size(dis,1),dis(:,i),'LineWidth',1);
end
xlabel("历元");
ylabel("距离(m)");
subplot(2,1,2);
hold on;
for i=1:width(dis_height)
    plot(1:size(dis_height,1),dis_height(:,i),'LineWidth',1);
end
legend("基站1","基站2","基站3","基站4","Location","bestoutside");
xlabel("历元");
ylabel("距离(m)");

%% UWB 2和3数据可以做一个对比 
clc;clear;
%读取文件
filePath_base="数据2-小车-UWB\base-station-location.txt";
filePath_rove="数据2-小车-UWB\range_tag_3.txt";
filePath_true="数据2-小车-UWB\true_position_tag_3.txt";

data_base=load(filePath_base);
data_rove=load(filePath_rove);
data_true=load(filePath_true);

keep = ismember(data_rove(:,1), data_true(:,1));   % 逻辑索引
data_rove = data_rove(keep, :);                 % 筛选后的数组

dis=data_rove(:,2:9);
time=data_rove(:,1);

for i=1:size(data_rove,1)
    [Q,result(i,:)]=trilaterate(data_base(:,2:4),dis(i,:),0);
end

rms_x=rms(result(:,1)-data_true(:,2));
rms_y=rms(result(:,2)-data_true(:,3));

figure;
plot(result(:,1), result(:,2),data_true(:,2),data_true(:,3));
xlabel('X(m)');
ylabel('Y(m)');
legend("定位结果","真值");
grid on;
axis equal;          % 关键：x、y 单位长度相同

figure;
hold on;
for i=1:width(dis)
    plot(1:size(dis,1),dis(:,i),'LineWidth',1);
end
legend("基站1","基站2","基站3","基站4","基站5","基站6","基站7","基站8");
xlabel("历元(s)");
ylabel("距离(m)");
%% 音频 range数据貌似少一列

%% 课上采集无遮挡 有遮挡和无遮挡可以对比
%读取文件
clc;clear;
filePath_base="数据4-课上同学采集-Ultrasonic\有遮挡数据\base-station-location.txt";
filePath_rove="数据4-课上同学采集-Ultrasonic\有遮挡数据\range.txt";
filePath_true="数据4-课上同学采集-Ultrasonic\有遮挡数据\ref_pos.txt";
filePath_baseno="数据4-课上同学采集-Ultrasonic\无遮挡数据\base-station-location.txt";
filePath_roveno="数据4-课上同学采集-Ultrasonic\无遮挡数据\range.txt";
filePath_trueno="数据4-课上同学采集-Ultrasonic\无遮挡数据\ref_pos.txt";

data_base=load(filePath_base);data_baseno=load(filePath_baseno);
data_rove=load(filePath_rove);data_roveno=load(filePath_roveno);
data_true=load(filePath_true);data_trueno=load(filePath_trueno);

keep = ismember(data_rove(:,1), data_true(:,1));   % 逻辑索引
data_rove = data_rove(keep, :);                 % 筛选后的数组
keepno = ismember(data_roveno(:,1), data_trueno(:,1));   % 逻辑索引
data_roveno = data_roveno(keepno, :);                 % 筛选后的数组

dis=data_rove(:,2:5);disno=data_roveno(:,2:5);
time=data_rove(:,1);timeno=data_roveno(:,1);

for i=1:size(data_rove,1)
    [Q(i,:,:),result(i,:)]=trilaterate(data_base(:,2:3),dis(i,:),0);
end
for i=1:size(data_roveno,1)
    [Qno(i,:,:),resultno(i,:)]=trilaterate(data_baseno(:,2:3),disno(i,:),0);
end

for i=1:size(data_rove,1)
    if (abs(result(i,1)-mean(result(:,1))))>3*std(result(:,1))
        result(i,1)=NaN;
    end
end

rms_x=rms(result(:,1)-data_true(:,2),'omitnan');rms_xno=rms(resultno(:,1)-data_trueno(:,2),'omitnan');
rms_y=rms(result(:,2)-data_true(:,3),'omitnan');rms_yno=rms(resultno(:,2)-data_trueno(:,3),'omitnan');

figure;
subplot(2,1,1);
hold on;
scatter(result(:,1), result(:,2),5,'filled')
scatter(data_true(:,2),data_true(:,3),3,'filled');
legend("定位结果","真实结果");
xlabel('X(m)');
ylabel('Y(m)');
title("(a)有遮挡数据");
hold off;
grid on;
axis equal;          % 关键：x、y 单位长度相同
subplot(2,1,2);
hold on;
scatter(resultno(:,1), resultno(:,2),5,'filled')
scatter(data_trueno(:,2),data_trueno(:,3),3,'filled');
legend("定位结果","真实结果");
xlabel('X(m)');
ylabel('Y(m)');
title("(b)无遮挡数据");
hold off;
grid on;
axis equal;          % 关键：x、y 单位长度相同

%所有历元平均水平方向误差椭圆
Q=mean(Q,1);
Qno=mean(Qno,1);
a1=sqrt(0.5*(Q(1,1,1)+Q(1,2,2))+sqrt(0.25*((Q(1,1,1)-Q(1,2,2))^2)+Q(1,2,1)));
a2=sqrt(0.5*(Q(1,1,1)+Q(1,2,2))-sqrt(0.25*((Q(1,1,1)-Q(1,2,2))^2)+Q(1,2,1)));
a1_height=sqrt(0.5*(Qno(1,1,1)+Qno(1,2,2))+sqrt(0.25*((Qno(1,1,1)-Qno(1,2,2))^2)+Qno(1,2,1)));
a2_height=sqrt(0.5*(Qno(1,1,1)+Qno(1,2,2))-sqrt(0.25*((Qno(1,1,1)-Qno(1,2,2))^2)+Qno(1,2,1)));
% 参数方程
theta = linspace(0, 2*pi, 100);
x = 0 + a1 * cos(theta);
y = 0 + a2 * sin(theta);
x_height = 0 + a1_height * cos(theta);
y_height = 0 + a2_height * sin(theta);

figure;
plot(x, y,x_height,y_height,'LineWidth', 2);
axis equal;  % 重要：保持纵横比相等
grid on;
xlabel('X(m)');
ylabel('Y(m)');
title('水平方向误差椭圆');
legend("有遮挡数据","无遮挡数据");
