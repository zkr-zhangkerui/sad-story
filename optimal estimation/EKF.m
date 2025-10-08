%%一步预测
%状态转移矩阵state_change 积分部分Omage 预测方差var
%X是上一个历元的滤波值
function [state_change,Omage,var]=time_predict(t0,t1,X)
    d_t=t1-t0;
    k_x=0.01;
    k_y=0.05;
    g=9.8;
    var_e=[1.5^2,0;0,1.5^2];%系统噪声协方差
    I=diag([1,1,1,1]);

    A=zeros(4,4);
    A(1,2)=1;A(2,2)=-2*k_x*X(2);A(3,4)=1;A(4,4)=2*k_y*X(4);
    state_change=I+A*d_t;%状态转移矩阵
    
    G=[0,k_x*X(2)^2,0,-k_y*X(4)^2-g];
    %计算积分部分
    f=@(t) (I+A.*(t-t0));
    Omage= integral(f,t0,t1,'ArrayValued', true);
    Omage=Omage*G';
    
    C=[0,0;1,0;0,0;0,1];
    %预测方差
    f1=@(t) ((I+A*(t-t0))*C*var_e*C'*(I+A*(t-t0))');
    var=integral(f1,t0,t1,'ArrayValued', true);
end

%%测量更新
%滤波方差var 滤波结果X 新息V
%0-扩展的卡尔曼滤波 1-观测值r更新 2-观测值α更新
function [var,X,V]=update(X,var,Z,choose)
    var_delat=[10,0;0,10^(-5)];%观测噪声协方差
    I=diag([1,1,1,1]);
    if choose==1
        H=[X(1)/sqrt(X(1)^2+X(3)^2),0,X(3)/sqrt(X(1)^2+X(3)^2),0];
        K=var*H'*inv(H*var*H'+var_delat(1,1));
        V=Z-sqrt(X(1)^2+X(3)^2);
        var=(I-K*H)*var*(I-K*H)'+K*var_delat(1,1)*K';
    elseif choose==2
        H=[X(3)/(X(1)^2+X(3)^2),0,-X(1)/(X(1)^2+X(3)^2),0];
        K=var*H'*inv(H*var*H'+var_delat(2,2));
        V=Z-atan(X(1)/X(3));
        var=(I-K*H)*var*(I-K*H)'+K*var_delat(2,2)*K';
    elseif choose==0
        H=[X(1)/sqrt(X(1)^2+X(3)^2),0,X(3)/sqrt(X(1)^2+X(3)^2),0;
            X(3)/(X(1)^2+X(3)^2),0,-X(1)/(X(1)^2+X(3)^2),0];
        K=var*H'*inv(H*var*H'+var_delat);
        V=(Z-[sqrt(X(1)^2+X(3)^2),atan(X(1)/X(3))])';
        var=(I-K*H)*var*(I-K*H)'+K*var_delat*K';
    end
    X=X+K*V;
end

clc;
clear all;

all_X=zeros(99,8);%滤波结果
all_Var=zeros(99,4);%滤波方差
all_V=zeros(99,4);%新息
all_K_V=zeros(99,8);%增益
Z_x=zeros(99,1);
Z_y=zeros(99,1);

% 数据文件的路径
filename='E:\新建文件夹\大二下\最优估计\observation.txt'; 
outfile='outpos.txt';

fid=fopen(filename,'r');
fop=fopen(outfile,'w+');

disp(filename);
if(fid==-1 || fop==-1)
    disp('can not find or open the file: %')
    'wrong';
end
num=0;%历元数

%非逐次更新使用
X0=[0;50;500;0];
var0=diag([100,100,100,100]);
%逐次更新的使用
X=[0;50;500;0];
Var=diag([100,100,100,100]);

tline=fgetl(fid);%跳过第一行
while ~feof(fid)
    num=num+1;%历元数
    tline=fgetl(fid);
    date=sscanf(tline,"%f %f");
    t=(num-1)/10;%观测时间

    %一步预测
    [state_change,Omage,var]=time_predict(t,t+0.1,X0);
    X0=state_change*X0+Omage;
    var0=state_change*var0*state_change'+var;
    %%测量更新
    [var_0,X_0,V_0]=update(X0,var0,[date(1),date(2)],0);

    %观测值逐次更新
    %一步预测
    [state_change,Omage,var]=time_predict(t,t+0.1,X);
    X=state_change*X+Omage;
    Var=state_change*Var*state_change'+var;
    %测量更新
    [var_1,X_1,V_1]=update(X,Var,date(1),1);
    [var_2,X_2,V_2]=update(X_1,var_1,date(2),2);

    all_X(num,:)=[X_0',X_2'];
    all_Var(num,:)=[var_0(1,1),var_0(3,3),var_2(1,1),var_2(3,3)];
    all_V(num,:)=[V_0',V_1,V_2];
    all_K_V(num,:)=[(X_0-X0)',(X_2-X)'];
    Z_x(num,1)=date(1)*sin(date(2));
    Z_y(num,1)=date(1)*cos(date(2));

    %作为下一次计算的初值
    X=X_2;Var=var_2;
    X0=X;var0=var_0;
end

%%画图
x=0:0.1:9.8;
%轨迹图
figure;
plot(all_X(:,1), all_X(:,3), 'LineWidth', 2); %非逐次更新的
hold on;
plot(all_X(:,5), all_X(:,7), 'LineWidth', 1); %逐次更新的
scatter(Z_x, Z_y, 20, 'filled', 'MarkerEdgeColor', 'k'); 
title('抛体的轨迹');
xlabel('X(m)');
ylabel('Y(m)');
legend('估计轨迹', '逐次更新','观测值');
grid on;
hold off;

%x，y的滤波方差
figure;
plot(x,all_Var(:,1), 'LineWidth', 2);
hold on;
plot(x,all_Var(:,2), 'LineWidth', 2);
plot(x,all_Var(:,3), 'LineWidth', 1);
plot(x,all_Var(:,4), 'LineWidth', 1);
title('x,y的滤波方差');
xlabel('t(s)');
ylabel('m^2');
legend('X的方差', 'Y的方差','X方差-逐次更新','Y方差-逐次更新');
grid on;
hold off;

%新息
figure;
subplot(2, 1, 1); 
plot(x, all_V(:,1),'LineWidth', 2);
hold on;
plot(x, all_V(:,3),'LineWidth', 1);
xlabel('t(s)');
ylabel('(m)');
grid on;
legend('r的新息','r的信息-逐次更新');

subplot(2, 1, 2); 
plot(x, all_V(:,2), 'LineWidth', 2);
hold on;
plot(x,all_V(:,4), 'LineWidth', 1);
title('观测值r和α的新息');
xlabel('t(s)');
ylabel('(rad)');
grid on;
legend('α的新息','α的信息-逐次更新');
title("观测值r和α的新息");

%增益
figure;
subplot(2, 2, 1); 
plot(x, all_K_V(:,1),'LineWidth', 2);
hold on;
plot(x,all_K_V(:,5),'LineWidth', 1);
xlabel('t(s)');
ylabel('(m)');
grid on;
legend('X的增益','X的增益-逐次更新');

subplot(2, 2, 2); 
plot(x, all_K_V(:,2),'LineWidth', 2);
hold on;
plot(x,all_K_V(:,6),'LineWidth', 1);
xlabel('t(s)');
ylabel('(m/s)');
grid on;
legend('X方向速度的增益','逐次更新');

subplot(2, 2, 3); 
plot(x, all_K_V(:,3), 'LineWidth', 2);
hold on;
plot(x,all_K_V(:,7), 'LineWidth', 1);
xlabel('t(s)');
ylabel('(m)');
grid on;
legend('Y的增益','Y的增益-逐次更新');

subplot(2, 2, 4); 
plot(x, all_K_V(:,4),'LineWidth', 2);
hold on;
plot( x,all_K_V(:,8),'LineWidth', 1);
xlabel('t(s)');
ylabel('(m/s)');
grid on;
legend('Y方向速度的增益','逐次更新');
title("观测值对状态的增益");
