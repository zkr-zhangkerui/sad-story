function xyz = llh2xyz(llh)
%LLH2XYZ  Convert from latitude, longitude and height
%         to ECEF cartesian coordinates.  WGS-84
%
%	xyz = LLH2XYZ(llh)	
%
%	llh(1) = latitude in degrees 纬度
%	llh(2) = longitude in degrees 经度
%	llh(3) = height above ellipsoid in meters
%
%	xyz(1) = ECEF x-coordinate in meters
%	xyz(2) = ECEF y-coordinate in meters
%	xyz(3) = ECEF z-coordinate in meters
	phi = llh(1);
	lambda = llh(2);
	h = llh(3);

	a = 6378137.0000;	% earth semimajor axis in meters
	b = 6356752.3142;	% earth semiminor axis in meters	
	e = sqrt (1-(b/a).^2);

	sinphi = sind(phi);
	cosphi = cosd(phi);
	coslam = cosd(lambda);
	sinlam = sind(lambda);
	tan2phi = (tand(phi))^2;
	tmp = 1 - e*e;
	tmpden = sqrt( 1 + tmp*tan2phi );

	x = (a*coslam)/tmpden + h*coslam*cosphi;

	y = (a*sinlam)/tmpden + h*sinlam*cosphi;

	tmp2 = sqrt(1 - e*e*sinphi*sinphi);
	z = (a*tmp*sinphi)/tmp2 + h*sinphi;

	xyz(1) = x;
	xyz(2) = y;
	xyz(3) = z;
end