function xyz = enu2xyz(enu, orgxyz)
%ENU2XYZ	Convert from rectangular local-level-tangent ('East'-'North'-Up) to WGS-84 ECEF cartesian coordinates coordinates.
% xyz = ENU2XYZ(enu,orgenu)
tmporg = orgxyz;
orgllh = xyz2llh(orgxyz);
phi = orgllh(1);
lam = orgllh(2);
sinphi = sind(phi);
cosphi = cosd(phi);
sinlam = sind(lam);
coslam = cosd(lam);
R = [ -sinlam          coslam         0     ; ...
      -sinphi*coslam  -sinphi*sinlam  cosphi; ...
       cosphi*coslam   cosphi*sinlam  sinphi];

difxyz = (R^(-1) * enu')';
tmpxyz(:,1) = difxyz(:,1) + tmporg(1);
tmpxyz(:,2) = difxyz(:,2) + tmporg(2);
tmpxyz(:,3) = difxyz(:,3) + tmporg(3);
xyz = tmpxyz;
end
