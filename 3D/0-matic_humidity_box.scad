compx = 87;	// Size of compartments, X
compy = 175;	// Size of compartments, Y
wall = 2;		// Width of wall
nox = 2;		// Number of compartments, X
noy = 1;		// Number of compartments, Y
deep = 150;		// Depth of compartments

tolerance=.05;	// Tolerance around lid.  If it's too tight, increase this. 
			// If it's too loose, decrease it.
lidoffset=3;		// This is how far away from the box to print the lid
lidheight=8;		// Height of lid (duh)
showlid=true;	// Whether or not to render the lid
showbox=false;	// Whether or not to render the box

if (showbox) {
difference() {
	cube ( size = [nox * (compx + wall) + wall, noy * (compy + wall) + wall, (deep + wall)], center = false);
    union() {
	for ( ybox = [ 0 : noy - 1])
		{
             for( xbox = [ 0 : nox - 1])
			{
            $fn=10;
			translate([ xbox * ( compx + wall ) + wall+5, ybox * ( compy + wall ) + wall+5, wall+5])
            minkowski() {
			cube ( size = [ compx-10, compy-10, deep-9 ]);
            sphere ( 5);
            }
			}
		}
	};
    
    translate([compx, compy/2, 30])
    rotate ([0,90,0])
    cylinder (r=9, h=20, center=true);
}
}

if (showlid) {
translate ([0,noy*(compy+wall)+wall+lidoffset,0])
difference() {
	cube ( size = [nox * (compx + wall) + 3 * wall + 2* tolerance, noy * (compy + wall) + 3 * wall + 2*tolerance, lidheight], center = false);
    union() {
	translate ([wall,wall,wall])
	cube ( size = [nox * (compx + wall) +wall+tolerance, noy * (compy + wall) + wall + tolerance, lidheight+1], center = false);
    translate ([compx/2, compy/2 ,0])
      cylinder (r=13.4, h=20, center=true); 
    translate ([compx/2+25, compy/2 ,0])
      cylinder (r=5, h=20, center=true);    
    }
	}

}