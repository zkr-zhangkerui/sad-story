function cal=gps2cal1(date,tow)
% gps2cal1	�ɹ������ں�gps��������㹫��GPSʱ��
%  cal=gps2cal1(date,tow)  ���صĹ�����1x6����6�зֱ�Ϊ������ʱ����
%  date��1x3����3�зֱ�Ϊ����������
%  tow��GPS������

mjd=cal2mjd(date);

% GPS��MJD44244��ʼ
week=floor((mjd-44244)/7);
cal=gps2cal([week,tow]);
