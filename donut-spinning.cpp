//Donut rendering program
//2020/10/02

#include<iostream>
#include<cmath>
#include<unistd.h>

using namespace std;


                        /*All of this math is derived from parametric equations for a torus of revolution.
                          The axis of revolution is the z axis in this case, and the "screen" is the x,y plane.
                          The "camera" also looks down in the negative z direction, so the most positive z values are prioritized
                          when selecting what to display on the screen.
                          
                          The original parametric equations (no "shape rotations" around Z or Y axes) for the torus used here are:
                                x = cos(u)*(LITTLE_RADIUS * cos(v) + BIG_RADIUS)
                                y = sin(u)*(LITTLE_RADIUS * cos(v) + BIG_RADIUS)
                                z = LITTLE_RADIUS * sin(v)
                          
                          To arrive at the post-"shape rotation" components simply multiply this parameterization by a rotation matrix, or two as 
                          is done in this case (this program rotates the torus around the Y and Z axes)
                          
                          The perspective adjustment scales the x,y values by the corresponding z values, shifting z to be strictly
                          positive (hence PERSPECTIVE_NUMBER should be greater than 1, as it is multiplied by the largest possible z
                          value to obtain PERSPECTIVE).

                          The variable light is the dot product of the surface normal of the torus with the vector (0.4,-1,1).
                          The surface normal of the torus can be somehwat intuitively interpreted as kind of like a sphere; setting 
                          BIG_RADIUS = 0 and LITTLE_RADIUS = 1 in the original parameterization of the torus yields a 
                          parameterization for the surface normal of the torus.
                          
                          Most of the values in the "light" calculation are analogous to the post-rotation components of the torus; the
                          other numbers at the end (1.8,3.4,8) serve to 'move' l to the interval [0,9], with 0 being "vectors almost completely
                          misaligned" and 9 being "vectors almost perfectly aligned."
                                                  
                          light is then used to select a "brightness" character from the array symbols.*/
                          

int main()
{
        //values to play around with.*************************************
        //
        //use these to change the appearance of the output.
        //
        //(-1 - 1, -1 is completely frosted; 0 is top half; 1 is not frosted.)
        const double FROSTING_HEIGHT = 0.2;
        char symbols[10] = {' ','-','.',',','~','+','=','a','#','@'};
        char background = ' ';
        //
        //
        const double BIG_RADIUS = 6;
        const double LITTLE_RADIUS = 3;
        //note that because each 'pixel' actually takes *two* 
        //characters you may need to double your window's width.
        const int FRAME_HEIGHT = 25;
        const int FRAME_WIDTH = 25;
        //
        const double Y_AXIS_SPEED = 0.07;
        const double Z_AXIS_SPEED = 0.03;
        const double FRAME_LENGTH_SECONDS = 0.02;
        //should be greater than 1, smaller values cause more distortion.
        //I recommend about 2.
        const double PERSPECTIVE_NUMBER = 2;
        //
        //darkness of non-frosted area increases with contrast.
        const double CONTRAST = 1.7;
        //***********************************************************
        //***********************************************************


        const double PERSPECTIVE = PERSPECTIVE_NUMBER*(LITTLE_RADIUS + BIG_RADIUS);
        const double FROSTING = FROSTING_HEIGHT*LITTLE_RADIUS;


        //important variables
        char screen[FRAME_HEIGHT][FRAME_WIDTH];
        double topzvalue[FRAME_HEIGHT][FRAME_WIDTH];
        double radiansY=0;
        double radiansZ=0;
        double x;
        double y;
        double z;
        double cosu;
        double cosv;
        double sinu;
        double sinv;
        double cosRadY;
        double sinRadY;
        double cosRadZ;
        double sinRadZ;
        double lcb;
        int light;
        int xInt;
        int yInt;

        while(true)
        {
                //clear screen character to start with a clean "canvas"
                cout << "\033[2J"; 
                for(int i=0;i<FRAME_HEIGHT;i++)
                {
                        for(int j =0;j<FRAME_WIDTH;j++)
                        {
                                screen[i][j] = background;
                                topzvalue[i][j] = -999;
                        }
                }




                //increments for u/v affect the "resolution" of the surface.
                //dark patches are usually a result of insufficient "resolution".
                for(double u = 0; u <= 6.3; u+=0.02)
                {
                        for(double v = 0; v <= 6.3; v+=0.02)
                        {
                                //computing once
                                cosu = cos(u);
                                cosv = cos(v);
                                sinu = sin(u);
                                sinv = sin(v);
                                cosRadY = cos(radiansY);
                                sinRadY = sin(radiansY);
                                cosRadZ = cos(radiansZ);
                                sinRadZ = sin(radiansZ);
                                lcb = LITTLE_RADIUS*cosv+BIG_RADIUS;


                                //calculating the light value
                                light = round((0.4*cosRadY*cosRadZ*cosu*cosv+0.4*sinRadZ*sinu*cosv+sinRadY*cosRadZ*sinv/*<-x */ \
                                +cosRadY*sinRadZ*cosu*cosv-cosRadZ*sinu*cosv+sinRadY*sinRadZ*sinv/*<-y */\
                                +cosRadY*sinv-sinRadY*cosu*cosv+1.8)/3.4*8);
                                //Where components "land" after rotation.
                                z = cosRadY*LITTLE_RADIUS*sinv-sinRadY*cosu*lcb;
                                x = FRAME_WIDTH/2 + PERSPECTIVE*(cosRadY*cosRadZ*cosu*lcb+sinRadZ*sinu*lcb+sinRadY*cosRadZ*LITTLE_RADIUS*sinv)/(z-PERSPECTIVE);
                                y = FRAME_HEIGHT/2 + PERSPECTIVE*(-cosRadY*sinRadZ*cosu*lcb+cosRadZ*sinu*lcb-sinRadY*sinRadZ*LITTLE_RADIUS*sinv)/(z-PERSPECTIVE);



                                //"building" the screen for this frame
                                if(y < FRAME_HEIGHT && y >= 0 && x < FRAME_WIDTH && x >= 0)
                                {
                                        xInt = floor(x);
                                        yInt = floor(y);
                                        //if an x,y value that the torus maps to has already been mapped, only take the highest z value ('closest') point.
                                        if(topzvalue[yInt][xInt] < z)
                                        {
                                                topzvalue[yInt][xInt] = z;
                                                //if (pre-rotation) z value is high enough; frosting.
                                                if(LITTLE_RADIUS*sinv > FROSTING)
                                                {
                                                        screen[yInt][xInt] = symbols[light];
                                                }
                                                //make the surface 1.5 times less 'bright' if not frosting.
                                                else
                                                {
                                                        screen[yInt][xInt] = symbols[(int)(light/CONTRAST)];
                                                }
                                        }
                                }
                        }
                }



                //print out the screen
                for(int i=0;i<FRAME_HEIGHT;i++)
                {
                        for(int j =0;j<FRAME_WIDTH;j++)
                        {
                                cout << screen[i][j] << ' ';
                        }
                        cout << endl;
                }


                //increment rotations
                radiansY+=Y_AXIS_SPEED;
                radiansZ+=Z_AXIS_SPEED;
                //sleep for frame length
                sleep(FRAME_LENGTH_SECONDS);
                //next frame...
        }
        return 0;
}
